#include "VulnerableSection.h"
#include "DETECTOR.h"
#include "P_DETECTOR.h"

/***************************
 * VULNERABLESECTION CLASS *
 ***************************/
VulnerableSection::VulnerableSection(DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn *lastCmpInsn)
: detector(detector), bb(bb), startInsn(startInsn), vulnerableInsns(vulnerableInsns), endInsn(endInsn), isExitSection(isExitSection), prevVulnerableSection(prevVulnerableSection), signature(signature), prependPoint(startInsn), appendPoint(endInsn), nextVulnerableSection(nullptr), lastCmpInsn(lastCmpInsn), repeatLastCmp(false)
{
    // If a vulnerable section is not the first one in the BB, set the previous one as the sole source
    // and mark this vulnerable section as the next one of the previous vulnerable section
    if (prevVulnerableSection)
    {
        sources.push_back(prevVulnerableSection);
        prevVulnerableSection->setNextVulnerableSection(this);
    }
}

// -- OPERATOR OVERLOADS --------------------------------------------------------------------------------------

bool VulnerableSection::operator==(const VulnerableSection &s2)
{
    return this->signature == s2.signature;
}
bool VulnerableSection::operator!=(const VulnerableSection &s2)
{
    return this->signature != s2.signature;
}
bool VulnerableSection::operator<(const VulnerableSection &s2)
{
    return this->signature < s2.signature;
}
bool VulnerableSection::operator>(const VulnerableSection &s2)
{
    return this->signature > s2.signature;
}
bool VulnerableSection::operator<=(const VulnerableSection &s2)
{
    return this->signature <= s2.signature;
}
bool VulnerableSection::operator>=(const VulnerableSection &s2)
{
    return this->signature >= s2.signature;
}

// -- GETTERS  ----------------------------------------------------------------------------------------------

rtx_insn *VulnerableSection::getLastCmpInsn(void)
{
    return lastCmpInsn;
}

basic_block &VulnerableSection::getBB(void)
{
    return bb;
}

const rtx_insn *const VulnerableSection::getStartInsn(void)
{
    return startInsn;
}

const rtx_insn *const VulnerableSection::getEndInsn(void)
{
    return endInsn;
}

VulnerableSection *VulnerableSection::getPrevVulnerableSection(void)
{
    return prevVulnerableSection;
}

VulnerableSection *VulnerableSection::getNextVulnerableSection(void)
{
    return nextVulnerableSection;
}

bool VulnerableSection::isFirstVulnerableSection(void)
{
    return prevVulnerableSection == nullptr;
}

bool VulnerableSection::isLastVulnerableSection(void)
{
    return nextVulnerableSection == nullptr;
}

bool VulnerableSection::isExit(void)
{
    return isExitSection;
}

unsigned int VulnerableSection::getSignature(void)
{
    return signature;
}

rtx_insn *VulnerableSection::getCheckpoint(void)
{
    return checkpoint;
}

// ------------------------------------------------------------------------------------------------------

bool VulnerableSection::inVulnerableSection(rtx_insn* insn)
{
    return std::count(vulnerableInsns.begin(), vulnerableInsns.end(), insn);
}

void VulnerableSection::repeatLastCmpInsn(bool enable)
{
    repeatLastCmp = enable;
}

void VulnerableSection::setPrevVulnerableSection(VulnerableSection* section)
{
    prevVulnerableSection = section;
    sources.push_back(prevVulnerableSection);
    prevVulnerableSection->setNextVulnerableSection(this);
}

void VulnerableSection::setNextVulnerableSection(VulnerableSection *section)
{
    nextVulnerableSection = section;
}

void VulnerableSection::removeSources(void)
{
    sources.clear();
    if (prevVulnerableSection)
    {
        sources.push_back(prevVulnerableSection);
    }
}

void VulnerableSection::setStartInsn(rtx_insn *insn)
{
    startInsn = insn;
}

void VulnerableSection::setEndInsn(rtx_insn * insn)
{
    endInsn = insn;
}

void VulnerableSection::setExit(bool exit)
{
    isExitSection = exit;
}

void VulnerableSection::addVulnerableInstruction(rtx_insn *insn)
{
    vulnerableInsns.push_back(insn);
}

void VulnerableSection::addSourceVulnerableSections(vector<VulnerableSection *> sections)
{
    sources.insert(sources.end(), sections.begin(), sections.end());

    // Using bubble sort to sort the sources:
    for(int i=0; i<sources.size(); i++)
    {
        for(int j=0; j<sources.size()-1-i; j++)
        {
            if(*sources[j] > *sources[j+1])
            {
                VulnerableSection *temp = sources[j];
                sources[j] = sources[j+1];
                sources[j+1] = temp;
            }
        }
    }
    if(sources.size() > 3)
    {
        printf("\x1b[34;1m<addSourceVulnerableSections>\x1b[0m\tVulnerableSection with signature %d has more than 3 sources. I might use the alternative implementation.\n", signature);
        detector->setAlternativeImplementation(true);
    }
}


rtx_insn *VulnerableSection::prependParityInit(void)
{
    prependPoint = AsmGen::emitMovRegInt(detector->getSsPtr1(), 0, prependPoint, bb, EMIT_BEFORE);
    return prependPoint;
}

rtx_insn *VulnerableSection::prependExecutionCountCheck(void)
{
    prependPoint = AsmGen::emitCmpRegInt(detector->getSignatureReg(), 0, prependPoint, bb, EMIT_BEFORE);
    AsmGen::emitBeq(compareLabel, prependPoint, bb, EMIT_AFTER);
    return prependPoint;
}

rtx_insn *VulnerableSection::prependSignatureCheck(void)
{
    if (sources.size() > 1)
    {
        prependPoint = AsmGen::emitCmpRegInt(detector->getSignatureReg(), sources.at(1)->getSignature(), prependPoint, bb, EMIT_BEFORE);
    }
    return prependPoint;
}

rtx_insn *VulnerableSection::prependBackup()
{
    prependPoint = insertBackup(prependPoint);
    return prependPoint;
}

rtx_insn *VulnerableSection::prependRestore(void)
{
    prependPoint = detector->getIsa().insertPop(detector->getUsedRegs(), detector->getSsPtr2(), prependPoint, bb, EMIT_BEFORE);
    AsmGen::emitMovRegReg(SP_REGNUM, detector->getSignatureReg(), prependPoint, bb, EMIT_AFTER);
    return prependPoint;
}

rtx_insn *VulnerableSection::prependMarkSecondExecution(void)
{
    prependPoint = AsmGen::emitMovRegInt(detector->getSignatureReg(), 0, prependPoint, bb, EMIT_BEFORE);
    return prependPoint;
}

rtx_insn *VulnerableSection::prependJumpToCheckpoint(bool alternative)
{
    size_t numSources = sources.size();
    if (numSources == 0)
    {
        printf("\x1b[91mVulnerable section with signature \x1b[1m%d\x1b[0m\x1b[91m without a source\x1b[0m\n", signature);
        throw "Vulnerable section without a source";
    }
    else if (numSources == 1)
    {
        prependPoint = AsmGen::emitB(sources.at(0)->checkpoint, prependPoint, bb, EMIT_BEFORE);
        return prependPoint;
    }
    else
    {
        if(alternative)
        {
            rtx_insn *after = nullptr;
            int remainingSources = numSources;
            while(remainingSources > 0)
            {
                size_t currentSource = numSources-remainingSources;
                if (remainingSources > 1)
                {
                    if (!after)             // if this is the first instruction being added
                    {                       // prepend the instruction & mark it as the new prependPoint
                        prependPoint = AsmGen::emitCmpRegInt(detector->getSignatureReg(), sources.at(currentSource+1)->signature, prependPoint, bb, EMIT_BEFORE);
                        after = prependPoint;
                    }
                    else
                    {
                        after = AsmGen::emitCmpRegInt(detector->getSignatureReg(), sources.at(currentSource+1)->signature, after, bb, EMIT_AFTER);
                    }
                    
                    if (remainingSources == 3)
                    {
                        after = AsmGen::emitBlo(sources.at(currentSource)->checkpoint, after, bb, EMIT_AFTER);
                        after = AsmGen::emitBeq(sources.at(currentSource+1)->checkpoint, after, bb, EMIT_AFTER);
                        after = AsmGen::emitBhi(sources.at(currentSource+2)->checkpoint, after, bb, EMIT_AFTER);
                        remainingSources -= 3;
                    }
                    else
                    {
                        after = AsmGen::emitBlo(sources.at(currentSource)->checkpoint, after, bb, EMIT_AFTER);
                        after = AsmGen::emitBeq(sources.at(currentSource+1)->checkpoint, after, bb, EMIT_AFTER);
                        remainingSources -= 2;
                    }
                }
                else
                {
                    after = AsmGen::emitB(sources.at(currentSource)->checkpoint, after, bb, EMIT_AFTER);
                    remainingSources--;
                }
            }
        }
        else
        {
            if (numSources == 2)
            {
                prependPoint = AsmGen::emitBne(sources.at(0)->checkpoint, prependPoint, bb, EMIT_BEFORE);
                AsmGen::emitBeq(sources.at(1)->checkpoint, prependPoint, bb, EMIT_AFTER);
            }
            else
            {
                prependPoint = AsmGen::emitBhi(sources.at(2)->checkpoint, prependPoint, bb, EMIT_BEFORE);
                rtx_insn *after = AsmGen::emitBeq(sources.at(1)->checkpoint, prependPoint, bb, EMIT_AFTER);
                AsmGen::emitBlo(sources.at(0)->checkpoint, after, bb, EMIT_AFTER);
            }
            return prependPoint;
        }
    }
}

rtx_insn *VulnerableSection::prependComparison(void)
{
    compareLabel = AsmGen::emitCodeLabel(0, prependPoint, bb, EMIT_BEFORE);
    prependPoint = compareLabel;
    rtx_insn *after = AsmGen::emitBl(detector->getCompareSubroutine(), prependPoint, EMIT_AFTER);
    AsmGen::emitMovRegReg(LR_REGNUM, detector->getSignatureReg(), after, bb, EMIT_AFTER);
    return prependPoint;
}

rtx_insn *VulnerableSection::appendCheckpoint(bool setLabel)
{
    bool after = true;
    while(!LABEL_P(appendPoint) && (CALL_P(appendPoint) ||
        !NONDEBUG_INSN_P(appendPoint) ||
        InstrType::isUnspecVolatile(appendPoint) ||
        InstrType::isUnspec(appendPoint) || 
        InstrType::isUse(appendPoint))) {
        // find the next instruction and prepend it instead
        after=false;
        appendPoint = next_insn(appendPoint);
    }
    appendPoint = AsmGen::emitMovRegReg(detector->getSignatureReg(), SP_REGNUM, appendPoint, bb, after);
    appendPoint = detector->getIsa().insertPush(detector->getUsedRegs(), detector->getSsPtr2(), appendPoint, bb, EMIT_AFTER);
    appendPoint = AsmGen::emitMovRegInt(detector->getSignatureReg(), signature, appendPoint, bb, EMIT_AFTER);
    if(setLabel)
    {
        appendPoint = AsmGen::emitCodeLabel(0, appendPoint, bb, EMIT_AFTER);
        checkpoint = appendPoint;
    }
    return appendPoint;
}

rtx_insn *VulnerableSection::appendOriginalCodeLabel(void)
{
    appendPoint = AsmGen::emitCodeLabel(0, appendPoint, bb, EMIT_AFTER);
    originalCode = appendPoint;
    return appendPoint;
}

rtx_insn *VulnerableSection::appendLastCmp(void)
{
    if (repeatLastCmp)
    {
        if (lastCmpInsn != nullptr)
        {
            // duplicate compare
            appendPoint = emit_copy_of_insn_after(lastCmpInsn, appendPoint);
            repeatLastCmp = false;
        }
        else
        {
            printf("\x1b[34;1m<appendLastCmp>\x1b[0m\t\t\\tx1b[91mWarning: unable to copy lastCmpInsn for VulnerableSection with signature \x1b[1m%d\x1b[0m (BB%d)\x1b[0m\n", signature, bb->index-2);
        }
    }

    return appendPoint;
}

rtx_insn* VulnerableSection::prependParityCheck(void)
{
    // mov lr r11
    // cmp r5, #0
    // bne error
    prependPoint = AsmGen::emitMovRegReg(LR_REGNUM, detector->getSignatureReg(), prependPoint, bb, EMIT_BEFORE);
    rtx_insn *after = AsmGen::emitCmpRegInt(detector->getSsPtr1(), 0, prependPoint, bb, EMIT_AFTER);
    AsmGen::emitBne(detector->getErrorLabel(), after, bb, EMIT_AFTER);

    return prependPoint;
}

rtx_insn* VulnerableSection::prependParity(bool setCompareLabel)
{
    // [<compareLabel>:]
    // mov r11, sp
    // eor r5, r11  --> we have to do it like this, because an eor operation on the sp is not allowed
    // mov r11, lr
    // bl compareSubroutine
    prependPoint = insertParity(prependPoint);
    if (setCompareLabel)
    {
        compareLabel = AsmGen::emitCodeLabel(0, prependPoint, bb, EMIT_BEFORE);
        prependPoint = compareLabel;
    }
    return prependPoint;
}

void VulnerableSection::insertParity(bool setCompareLabel)
{
    insertParity(appendPoint);
    if (setCompareLabel)
    {
        compareLabel = AsmGen::emitCodeLabel(0, appendPoint, bb, EMIT_BEFORE);
    }
}

void VulnerableSection::insertJumpToOriginalCode(void)
{
    AsmGen::emitB(originalCode, appendPoint, bb, EMIT_BEFORE);
    checkpoint = AsmGen::emitCodeLabel(0, appendPoint, bb, EMIT_BEFORE);
}

void VulnerableSection::insertRestore(void)
{
    rtx_insn *inserted = detector->getIsa().insertPop(detector->getUsedRegs(), detector->getSsPtr2(), appendPoint, bb, EMIT_BEFORE);
    AsmGen::emitMovRegReg(SP_REGNUM, detector->getSignatureReg(), inserted, bb, EMIT_AFTER);
}

void VulnerableSection::insertBackup(void)
{
    insertBackup(appendPoint);
}

rtx_insn *VulnerableSection::insertBackup(rtx_insn *beforeInsn) {
    rtx_insn *first;

    if(detector->getSpProtected()) {
        first = AsmGen::emitMovRegReg(detector->getSignatureReg(), SP_REGNUM, beforeInsn, bb, EMIT_BEFORE);
        detector->getIsa().insertPush(detector->getProtectedRegs(), detector->getSsPtr1(), first, bb, EMIT_AFTER);  
    } else {
        return detector->getIsa().insertPush(detector->getProtectedRegs(), detector->getSsPtr1(), beforeInsn, bb, EMIT_BEFORE);  
    }
    return first;
}

rtx_insn *VulnerableSection::insertParity(rtx_insn *beforeInsn) {
    rtx_insn *first;
    rtx_insn *after;
    if(detector->getSpProtected()) {
        first = AsmGen::emitMovRegReg(detector->getSignatureReg(), SP_REGNUM, beforeInsn, bb, EMIT_BEFORE);
        after = AsmGen::emitEorRegReg(detector->getSsPtr1(), detector->getSsPtr1(), detector->getSignatureReg(), first, bb, EMIT_AFTER);
        after = AsmGen::emitMovRegReg(detector->getSignatureReg(), LR_REGNUM, after, bb, EMIT_AFTER);
    } else {
        first = AsmGen::emitMovRegReg(detector->getSignatureReg(), LR_REGNUM, beforeInsn, bb, EMIT_BEFORE);
        after = first;
    }    
    AsmGen::emitBl(detector->getCompareSubroutine(), after, EMIT_AFTER);
    return first;
}

void VulnerableSection::insertMarkSecondExecution(void)
{
    AsmGen::emitMovRegInt(detector->getSignatureReg(), 0, appendPoint, bb, EMIT_BEFORE);
}
