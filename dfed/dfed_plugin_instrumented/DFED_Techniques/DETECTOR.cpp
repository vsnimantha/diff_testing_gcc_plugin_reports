#include "DETECTOR.h"

DETECTOR::DETECTOR(ARM_ISA *isa, map<unsigned char, unsigned char> regmap, unsigned char signatureReg, unsigned char ssPtr1, unsigned char ssPtr2, vector<unsigned char> protectedRegs)
: GeneralDFED(isa, regmap)
{
    this->signatureReg = signatureReg;
    this->ssPtr1 = ssPtr1;
    this->ssPtr2 = ssPtr2;
    // this->protectedRegs becomes a copy of protectedRegs, but:
    // - with SP_REGNUM replaced by signatureReg
    // - with LR_REGNUM added
    this->protectedRegs.reserve(protectedRegs.size()+1);
    this->spProtected = false;
    for (unsigned char reg : protectedRegs)
    {
        if (reg == SP_REGNUM)
        {
            this->protectedRegs.push_back(signatureReg);
            this->spProtected = true;
        }
        else
        {
            this->protectedRegs.push_back(reg);
        }
    }
    this->protectedRegs.push_back(LR_REGNUM);
    std::sort(this->protectedRegs.begin(), this->protectedRegs.end());
    this->CMPlimit = 254;
    this->compareSubroutine = "DETECTOR_compare";
    this->alternativeImplementation = false;
    this->errorLabel = nullptr;
    startPoint = nullptr;
}

void DETECTOR::implementTechnique(void)
{
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 1: findUsedRegisters ----------------------------\n");
    findUsedRegisters();            // find all the registers that are used in the program
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 2: insertSetup ----------------------------------\n");
    insertSetup();                  // add the first set of instructions
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 3: findVulnerableSections -----------------------\n");
    findVulnerableSections();       // find the vulnerable sections in the program
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 4: findReExecutionPoints ------------------------\n");
    findReExecutionPoints();        // find the re-execution points
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 5: findVulnerableSections from str [sp] ---------\n");
    findVulnerableSections(true);   // find the VulnerableSections defined by the stores to sp
    printf("\x1b[34;1m<implementTechnique>\x1b[0m\t\t---------------------------- Step 6: insertInstructions ---------------------------\n");
    insertInstructions();           // insert the instructions that make up the DETECTOR technique
}

void DETECTOR::setAlternativeImplementation(bool enable)
{
    alternativeImplementation = enable;
}

// -- GETTERS  ------------------------------------------------------------------------------------------------
unsigned char DETECTOR::getSignatureReg(void)
{
    return signatureReg;
}
unsigned char DETECTOR::getSsPtr1(void)
{
    return ssPtr1;
}
unsigned char DETECTOR::getSsPtr2(void)
{
    return ssPtr2;
}
ARM_ISA &DETECTOR::getIsa(void)
{
    return *isa;
}
vector<unsigned char> &DETECTOR::getUsedRegs(void)
{
    return usedRegs;
}
vector<unsigned char> &DETECTOR::getProtectedRegs(void) {
    return protectedRegs;
}
bool DETECTOR::getSpProtected(void) { return spProtected; }
char *DETECTOR::getCompareSubroutine(void) { return compareSubroutine; }

rtx_insn *DETECTOR::getErrorLabel(void)
{
    return errorLabel;
}

VulnerableSection *DETECTOR::getVulnerableSection(rtx_insn *insn)
{
    for (VulnerableSection *vs : vulnerableSections)
    {
        if (vs->inVulnerableSection(insn))
        {
            return vs;
        }
    }
    return nullptr;
}

VulnerableSection *DETECTOR::createVulnerableSection(basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn* lastCmpInsn) {
    return new VsDETECTOR(this, bb, startInsn, vulnerableInsns, endInsn, isExitSection, prevVulnerableSection, signature, lastCmpInsn);
}

// -- implementTechnique ---------------------------------------------------------------------------------------------------

void DETECTOR::findUsedRegisters(void)
{
    // TODO: Ideally, only the used registers should be backed-up.
    // however, I currently have no idea how to implement this (in the compare subroutine + accross multiple protected functions)
    // so, this is the temporary solution: save all registers except ssPtr1 & ssPtr2
    for (unsigned char i = LR_REGNUM; i <= LR_REGNUM; --i)
    {
        if (i != ssPtr1 && i != ssPtr2 && i != SP_REGNUM)
        {
            usedRegs.push_back(i);
        }
    }
}

void DETECTOR::insertSetup(void)
{
    basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);         // the entry basic block
    rtx_insn *firstInsn = UpdatePoint::firstRealINSN(bb); // the first instruction in the program
    unsigned int startSignature = generateSignature();
    
    // AsmGen::emitMovRegReg(signatureReg, SP_REGNUM, firstInsn, bb, EMIT_BEFORE);                     // MOV signatureReg, sp
    // isa->insertPush(getUsedRegs(), ssPtr2, firstInsn, bb, EMIT_BEFORE);                             // STMDB ssPtr2!, {usedRegs}
    // AsmGen::emitMovRegInt(signatureReg, signatures.at(0), firstInsn, bb, EMIT_BEFORE);              // MOV signatureReg, signature
    // rtx_insn *startLabel = AsmGen::emitCodeLabel(0, firstInsn, bb, EMIT_BEFORE);                    // checkpoint

    // the start point is also an instance of the VulnerableSection class, although it is not really a Vulnerable Section.
    // It just makes things easier
    // This VulnerableSection object is, however, not included in the vulnerableSection list
    rtx_insn *start = AsmGen::emitCodeLabel(0, firstInsn, bb, EMIT_BEFORE);
    startPoint = createVulnerableSection(bb, start, (vector<rtx_insn *>)NULL, start, false, nullptr, startSignature, nullptr);
    printf("\x1b[34;1m<insertSetup>\x1b[0m\t\t\t⌈ \x1b[1mØ\x1b[0m\t⌉\n");
    printf("\x1b[34;1m<insertSetup>\x1b[0m\t\t\t| %d\t|\n", startPoint->getSignature());
    printf("\x1b[34;1m<insertSetup>\x1b[0m\t\t\t⌊_______⌋\n", startPoint->getSignature());
}

void DETECTOR::findVulnerableSections(bool strSp)
{
    basic_block bb;
    FOR_EACH_BB_FN(bb, cfun)
    {
        printf("\x1b[34;1m<findVulnerableSections>\x1b[0m\t⌈ \x1b[1mBB%d\x1b[0m\t⌉\n",bb->index-2);
        rtx_insn *insn;
        rtx_insn *vulnerableStart = nullptr;                    // The start of the new VulnerableSection
        rtx_insn *vulnerableEnd = nullptr;                      // Te end of the new VulnerableSection
        vector<rtx_insn*> vulnerableInsn;                       // All the vulnerable instructions for a single BB
        rtx_insn *lastCompare = nullptr;                        // A condition might influence a vulnerable instruction later on
        VulnerableSection *lastVulnerableSection = nullptr;     // The last vulnerable section in this BB
        bool repeatCompare = false;                             // A conditional instruction might rely on a condition in/before the vulnerable section
        bool isExit = false;                                    // The VulnerableSecion might be an return
        bool added = false;                                     // Keep track of wether a VulnerableSection was added in this BB
        bool prevWasAlreadyProtected = false;                    // Remember if the last instruction we passed was one that was already protected
        FOR_BB_INSNS(bb, insn)
        {
            if (NONDEBUG_INSN_P(insn) && !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn) && !InstrType::isUse(insn))
            {
                bool isVulnerable = isVulnerableInsn(insn, bb, strSp);
                if (isVulnerable)
                {           
                    VulnerableSection * alreadyProtected = getVulnerableSection(insn);
                    if(!alreadyProtected) {                     // if we are discovering a vulnerable instruction ..
                        if(prevWasAlreadyProtected)             // .. right after an instruction that is protected ..
                        {                                       // .. add this vulnerable instruction to the VulnerableSection
                            lastVulnerableSection->addVulnerableInstruction(insn);
                            lastVulnerableSection->setEndInsn(insn);
                            if (InstrType::isCondExec(insn))
                            {
                                lastVulnerableSection->setStartInsn(lastCompare);
                            }
                            if (InstrType::isReturn(insn))
                            {
                                lastVulnerableSection->setExit(true);
                            }
                        }                                      
                        else                                    // if it is not after an existing one, treat it as a ..
                        {                                       // .. completely new one (for now):
                            if (InstrType::isCondExec(insn))    // if the vulnerable instruction is conditional, ..
                            {                                   // .. the previous compare instruction should be ..
                                vulnerableStart = lastCompare;  // .. the start of the vulnerable instruction
                            }
                            else if (!vulnerableStart)          // if we aren't already in a VulnerableSection ..
                            {                                   // ..
                                vulnerableStart = insn;         // .. the VulnerableSection starts here
                            }
                            if (InstrType::isReturn(insn))      // if the vulnerable instruction is a return ..
                            {                                   // ..
                                isExit = true;                  // .. mark the VulnerableSection as an exit
                            }
                            vulnerableEnd = insn;               // The VulnerableSection ends here (for now)
                            vulnerableInsn.push_back(insn);
                        }
                    } else {                                    // if this instruction is already protected ..
                        if (vulnerableStart)                    // .. and we are in the middle of adding a vulnerableSection ..
                        {                                       // .. add the instructions we were adding to the existing vulnerableSection
                            for(rtx_insn * vi : vulnerableInsn)
                            {
                                alreadyProtected->addVulnerableInstruction(vi);
                            }
                            alreadyProtected->setStartInsn(vulnerableStart);
                            vulnerableStart = nullptr;
                            vulnerableEnd = nullptr;
                            vulnerableInsn.clear();
                        }
                        if (added && alreadyProtected != lastVulnerableSection)
                        {
                            alreadyProtected->setPrevVulnerableSection(lastVulnerableSection);
                            // printf("\x1b[34;1m<findVulnerableSections>\x1b[0m\tThe predecessor of VB \x1b[1m%d\x1b[0m is now VB \x1b[1m%d\x1b[0m\n", alreadyProtected->getSignature(), lastVulnerableSection->getSignature());
                        }
                        lastVulnerableSection = alreadyProtected;
                        prevWasAlreadyProtected = true;
                    }
                    
                } else {
                    prevWasAlreadyProtected = false;
                }
                if (vulnerableStart && (!isVulnerable || isLastInsn(insn, bb)))
                // FIXME: If the lastInsn is a DEBUG_INSN/unspecVolatile/Unspec/inUse, the VulnerableSection will not close? (untested, possible bug)
                
                {                                               // if this instruction is the end of the vulnerable Section ..
                                                                // .. create the VulnerableSection object
                    unsigned int signature = generateSignature();
                    lastVulnerableSection = createVulnerableSection(bb, vulnerableStart, vulnerableInsn, vulnerableEnd, isExit, lastVulnerableSection, signature, lastCompare);
                    added = true;
                    vulnerableSections.push_back(lastVulnerableSection);
                    vulnerableStart = nullptr;                  // next vulnerable instruction will be a new VulnerableSection
                    printf("\x1b[34;1m<findVulnerableSections>\x1b[0m\t| %d\t|\n",lastVulnerableSection->getSignature());
                    repeatCompare = (bool)lastCompare;          // we might have to repeat the last compare (but not sure yet)
                    
                }
                if (InstrType::isCompare(insn) || InstrType::isParallelCompare(insn))
                {                                               // if this instruction is a compare ..
                    lastCompare = insn;                         // .. remember it for later
                    if (!vulnerableStart)                       // | if the cmp is outside a VulnerableSection ..
                    {                                           // | .. the cmp should not be repeated ..
                        repeatCompare = false;                  // | .. (unless maybe when a new VulnerableSection is found)
                    }
                }
                if (repeatCompare && (InstrType::isCondExec(insn) || InstrType::isCondJump(insn)))
                {                                               // if we encounter a conditional instruction and the ..
                    lastVulnerableSection->repeatLastCmpInsn(); // .. we know for sure if the previous compare should ..
                                                                // .. in fact be repeated if it is marked as such ..
                                                                // .. because this conditional instruction relies on ..
                                                                // .. the compare in/before the vulnerable setcion
                    printf("*"); // a star indicates that the last cmp should be copied
                }
            }
        }
        
        printf("\x1b[34;1m<findVulnerableSections>\x1b[0m\t⌊_______⌋\n\n", startPoint->getSignature());
        if (strSp && added)                                     // after adding one or more VulneableSections in a BB due to a str[sp] ..
        {   // TODO this is better to be done after each additon of VulnerableSection (if strSp) AND if at the end of the BB if that BB was inserted
            cleanReExecutionPoints();                           // ... the reExecution points should be recalculated
            findReExecutionPoints();
        }
    }
}

void DETECTOR::cleanReExecutionPoints(void)
{
    // printf("\x1b[34;1m<cleanReExecutionPoints>\x1b[0m\tClearing re-execution points..\n");
    for (VulnerableSection *section : vulnerableSections)
    {
        section->removeSources();
    }
    setAlternativeImplementation(false);
}

void DETECTOR::findReExecutionPoints(void)
{
    for (VulnerableSection *section : vulnerableSections)
    {
        if (section->isFirstVulnerableSection())
        {
            // printf("\x1b[34;1m<findReExecutionPoints>\x1b[0m\t\tFinding the sources of the first VulnerableSection in BB%d (signature \x1b[1m%d\x1b[0m): [", section->getBB()->index-2, section->getSignature());
            printf("\x1b[34;1m<findReExecutionPoints>\x1b[0m\t\t\tsources of (BB%d:%03d):\t", section->getBB()->index-2, section->getSignature());
            vector<int> traversed;
            vector<VulnerableSection *> sources;

            findSources(section->getBB(), vulnerableSections, traversed, sources);
            for(VulnerableSection *source : sources)
            {
                printf("(BB%d:%03d) ", source->getBB()->index-2, source->getSignature());
            }
            section->addSourceVulnerableSections(sources);
            printf("\n");
        }
    }
}

void DETECTOR::insertInstructions(void)
{
    insertStart();
    for (VulnerableSection *section : vulnerableSections) {
        if(!section->isExit()) {
          section->appendInsn(alternativeImplementation);
        }
    }
    for (VulnerableSection *section : vulnerableSections) {
        section->prependInsn(alternativeImplementation);
    }

}

// ------------------------------------------------------------------------------------------------------------------

void DETECTOR::markUsedRegisters(rtx instruction)
{
    if (instruction == 0x00)
    {
        return;
    }
    rtx_code exprCode = (rtx_code)instruction->code;
    if (exprCode == ASM_OPERANDS)
    {
        return;
    }
    if (REG_P(instruction))
    {
        unsigned char regNum = XINT(instruction, 0); // Get the register number
        usedRegMap[regNum]++;                        // Update the used register map
    }
    else
    {
        const char *format = GET_RTX_FORMAT(instruction->code);
        for (int x = 0; x < GET_RTX_LENGTH(instruction->code); x++)
        { // Loop over all characters in the format
            if (format[x] == 'e')
            {                                       // Test if current character indicates an expression
                rtx subExpr = XEXP(instruction, x); // Get the expression
                markUsedRegisters(instruction);     // recursive call to this function, with the sub expression
            }
            else if (format[x] == 'E')
            { // Test if a Vector
                for (int i = 0; i < XVECLEN(instruction, 0); i++)
                {                                             // Loop over all expression in the vector
                    rtx subExpr = XVECEXP(instruction, 0, i); // Get the expression
                    markUsedRegisters(instruction);           // recursive call to this function, with the sub expression
                }
            }
        }
    }
}

unsigned int DETECTOR::generateSignature(void)
{
    unsigned int tempSig = 0;
    do
    {
        tempSig = (rand() % CMPlimit) + 1;
    } while (!isUniqueSignature(tempSig) || tempSig == 0);

    signatures.push_back(tempSig);
    return tempSig;
}

bool DETECTOR::isUniqueSignature(unsigned int signature)
{
    for (auto const &existingSignature : signatures)
    {
        if (signature == existingSignature)
        {
            return false;
        }
    }
    return true;
}

bool DETECTOR::isVulnerableInsn(rtx_insn *insn, basic_block bb, bool strSp)
{
    if ( strSp && (!InstrType::isStore(insn, true) && InstrType::isStore_SP(insn, true)) )
    {
        
        unsigned char reg = InstrType::getStore_SpReg(insn); // get register
        int offset = InstrType::getStore_SpOffset(insn);     // get offset
        if( reg<255 )
        {
            // printf("\x1b[34;1m<isVulnerableInsn>\x1b[0m\t\tFound str r%d, [sp, #%d] ",reg, offset);
            rtx_insn *prevLdr = findPrevLdrSpInReExecution(reg, offset, bb, insn);
            // printf(prevLdr ? "(protected because of previous ldr)\n" : "(unprotected)\n");
            return prevLdr;
        }
        else
        {
            printf("\x1b[34;1m<isVulnerableInsn>\x1b[0m\t\t[warning] Could not find the str [sp] register. Ignoring...\n");
        }
    }
    return InstrType::isStore(insn, true) || CALL_P(insn) || InstrType::isReturn(insn);
}

rtx_insn *DETECTOR::findPrevLdrSpInReExecution(unsigned char reg, int offset, basic_block bb, rtx_insn *startInsn)
{
    bool searchOngoing = !startInsn;
    rtx_insn * insn;
    FOR_BB_INSNS_REVERSE(bb, insn)                          // iterate backwards over this bb, starting from this insn ..
    {
        if( insn == startInsn)
        {
            searchOngoing = true;                           // .. starting from the startInsn
        }
        if(searchOngoing)
        {   
            if( getVulnerableSection(insn) )                // if the insn is in a vulnerable section ..
            {
                return nullptr;                             // .. no ldr was found
            }
            else if( reg == InstrType::getLoad_SpReg(insn) && offset == InstrType::getLoad_SpOffset(insn) )
            {
                return insn;
            }
        }
    }
    
    // find all incoming BB's
    vec<edge, va_gc> *incomingEdges = bb->preds;
    edge e;
    edge_iterator ei;
    FOR_EACH_EDGE(e, ei, incomingEdges)
    {
        // search through the incoming bb's
        if(rtx_insn *foundLdr = findPrevLdrSpInReExecution(reg, offset, e->src))
        {
            return foundLdr;
        }
    }
    return nullptr;
}


bool DETECTOR::isLastInsn(rtx_insn *instruction, basic_block bb)
{
    return UpdatePoint::lastRealINSN(bb) == instruction;
}

vector<VulnerableSection *> DETECTOR::findSources(basic_block const &bb, vector<VulnerableSection *> const &vulnerableSections, vector<int> &traversed, vector<VulnerableSection *> &foundSources)
{
    // find all incoming BB's
    vec<edge, va_gc> *incomingEdges = bb->preds;
    edge e;
    edge_iterator ei;

    FOR_EACH_EDGE(e, ei, incomingEdges)
    {
        // traverse the source BBs
        basic_block source = e->src;
        // If this source BB has not yet been traversed
        if (find(traversed.begin(), traversed.end(), source->index) == traversed.end())
        {
            traversed.push_back(source->index);

            // add to sources if this block has a vulnerable section
            bool foundSource = false;
            for (VulnerableSection *section : vulnerableSections)
            {
                basic_block bb = section->getBB();
                if (bb->index == source->index && section->isLastVulnerableSection()) // found a source
                {
                    foundSources.push_back(section);
                    foundSource = true;
                    break;
                }
            }
            if (!foundSource && bb->index == startPoint->getBB()->index)
            {
                // the source is the start of the function
                foundSources.push_back(startPoint);
                foundSource = true;
            }
            if (!foundSource)
            {
                // recursive call to the function, continue to traverse the tree
                findSources(source, vulnerableSections, traversed, foundSources);
            }
        }
    }
    return foundSources;
}

void DETECTOR::insertStart(void)
{
    startPoint->insertStart(alternativeImplementation);
}
