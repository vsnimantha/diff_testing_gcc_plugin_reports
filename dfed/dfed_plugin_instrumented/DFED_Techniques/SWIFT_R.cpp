#include "SWIFT_R.h"

SWIFT_R::SWIFT_R(ARM_ISA* isa, map<unsigned char, unsigned char> regMap) 
: SWIFT(isa, regMap)
{}

void SWIFT_R::implementDuplication(rtx_insn* codeLabel) {
    
    basic_block bb;
    
    FOR_EACH_BB_FN(bb, cfun) {
        
		rtx_insn* insn;

        FOR_BB_INSNS(bb, insn) {

            if( should_duplicate(insn) ) {

                // FIRST DUPLICATE
				 
                rtx_insn* duplicate = emit_copy_of_insn_after(insn, insn);  // Make a duplicate instruction, replace registers further down
                duplicate->frame_related = 0;                               // Necessary since duplicate does not use stack pointer
                                
                //  Replace registers for duplicate instruction, based on regMap
                bool isFunctionArgument = InstrType::isLdrSPargument(insn, this->functionArgumentsFromFifth);
                findAndReplaceRegisters(XEXP(duplicate,3), XEXP(duplicate,3), !isFunctionArgument, 0); // offset = 0
                
                // SECOND DUPLICATE

                rtx_insn* duplicate2 = emit_copy_of_insn_after(insn, duplicate);    // make a second duplication
                duplicate2->frame_related = 0;

                // Replace registers for second duplicate instruction, based on regMap + 1
                findAndReplaceRegisters(XEXP(duplicate2,3), XEXP(duplicate2,3), !isFunctionArgument, 1); // offset = 1
                // An offset of 1 will replace the register with the register of the regMap + 1

                insn = duplicate2;

            } else if ( InstrType::isPop(insn) ) {

                if ( !InstrType::isReturn(insn) ) { // If the pc is popped, it is considered a return
                    // Copy over values from original pop to shadow registers
                    insn = update_shadow_regs_after_pop(insn, bb);
                }
            
            } else if ( InstrType::isPush(insn) ) {

                // Update shadow SP
                insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, insn, bb, true);
                insn = AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, insn, bb, true);

            }
        }
	}
}

void SWIFT_R::listUnsafeAreasAndCriticalInstructions(basic_block bb, std::map<uint, uint>* unsafeAreas, std::map<uint, rtx_insn*>* addCmpBeforeInsn, std::map<uint, rtx_insn*>* criticalInsn) {
    rtx_insn* insn;
    uint position = 0;
    
    rtx_insn* cmpInsn = nullptr;
    uint cmpPos = 0;
    char duplicatedCmp = 0;
    bool alreadyProtected = false;
    FOR_BB_INSNS(bb, insn) {
            // Skip instructions dat don't matter
        if(NONDEBUG_INSN_P(insn) && !InstrType::isUse(insn) && !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn)) {

            // remember the last instruction were the flag is updated
            if (InstrType::isCompare(insn) || InstrType::isParallelCompare(insn) || InstrType::isCBZ(insn)){ // TODO BDB: What if isCBNZ?

                if (!duplicatedCmp) { // Don't add cmp/bne instructions between a comparison and it's duplicate
                    cmpInsn =  insn;        // remember the last instruction were the flag is updated ..
                    cmpPos = position;      // .. and remember its position within the BB
                    
                    if (should_duplicate(insn)) {
                        duplicatedCmp = 2;  // Mark the next two instructions as a duplicated cmp
                    }
                } else {
                    duplicatedCmp--;    // Next instruction is either the second duplicate (duplicatedCmp = 1) or no duplicate (duplicatedCmp = 0)
                }
            }

            // save the unsafe areas & the critical instructions
            if (should_insert_comparison(insn)) {
                if (InstrType::isCondExec(insn) || InstrType::isCondJump(insn)) {
                    // For conditional critical instructions:
                    // No cmp/bne instructions should be inserted between the last cmp and the critical instruction
                    // unsafeArea = from cmp instruction until critical instruction

                    if (cmpInsn != nullptr) {
                        unsafeAreas->insert(std::pair<uint,uint>(cmpPos, position));             // add unsafe area
                        addCmpBeforeInsn->insert(std::pair<uint,rtx_insn*>(cmpPos, cmpInsn));    // add cmp instruction
                        criticalInsn->insert(std::pair<uint,rtx_insn*>(cmpPos, insn));           // add critical instruction

                    } else {
                        // We have a conditional execution / jump without a condition: This should not happen!
                        printf("\x1b[91m[WARNING]: No cmp found for instruction %u.\n\x1b[0m", position);
                    }

                } else { // only the first instruction in a "critical block" should be protected
                    // For non-conditional instructions:
                    // We don't really have an unsafe area, so start == end

                    unsafeAreas->insert(std::pair<uint,uint>(position, position));             // add unsafe area
                    addCmpBeforeInsn->insert(std::pair<uint,rtx_insn*>(position, insn));       // add critical instruction
                    criticalInsn->insert(std::pair<uint,rtx_insn*>(position, insn));           // |
                    
                }
            }
            position++;
        }
    }
}

void SWIFT_R::handleCall(rtx_insn* insn, basic_block bb){
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, insn, bb, false);
			AsmGen::emitMovRegReg(regMap[citr->first]+1, citr->first, insn, bb, false);
		}
	}
}

rtx_insn* SWIFT_R::update_shadow_regs_after_pop(rtx_insn* pop_insn, basic_block bb) {
    vector<unsigned char> usedRegs;
	find_all_used_registers(pop_insn, &usedRegs);
    
    // For every register that is popped
    for(auto &reg : usedRegs) {
        // Insert MOV <shadow reg> <original reg>, after pop_insn;
        pop_insn = AsmGen::emitMovRegReg(regMap[reg], reg, pop_insn, bb, true);
        pop_insn = AsmGen::emitMovRegReg(regMap[reg]+1, reg, pop_insn, bb, true);
    }

    pop_insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, pop_insn, bb, true);
    pop_insn = AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, pop_insn, bb, true);

    return pop_insn;
}

void SWIFT_R::insertSetup(){
    basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
	rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0 && citr->first != SP_REGNUM){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, prev, bb, false);
			AsmGen::emitMovRegReg(regMap[citr->first]+1, citr->first, prev, bb, false);
		}
	}
    // Sync the sp as well
    AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, prev, bb, false);
    AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, prev, bb, false);
}

void SWIFT_R::insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel) {
    // Find the used registers
    vector<unsigned char> usedRegs;
    bool spIsUsed = false;
    if(CALL_P(criticalInsn) || InstrType::isReturn(criticalInsn)) {
        // All used registers should be checked
        for(auto citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
            if (citr->second > 0){
                usedRegs.push_back(citr->first);
            }
        }
        spIsUsed = true;
    } else {
        // Only the registers used in the instructions should be checked
        find_all_used_registers(criticalInsn, &usedRegs);
        find_all_used_registers(beforeInsn, &usedRegs);
    }

    for(auto &reg : usedRegs){
        if (!std::count(protected_regs->begin(), protected_regs->end(), reg)){
            // protect this register
            rtx_insn* mov3 = AsmGen::emitMovRegReg(reg, regMap[reg], beforeInsn, bb, false);
            rtx_insn* equal = AsmGen::emitCodeLabel(0, mov3, bb, false);
            rtx_insn* mov2 = AsmGen::emitMovRegReg(regMap[reg]+1, reg, equal, bb, false);
            rtx_insn* mov1 = AsmGen::emitMovRegReg(regMap[reg], reg, mov2, bb, false);
            rtx_insn* beqInsn = AsmGen::emitBeq(equal, mov1, bb, false);
            rtx_insn* cmpInsn = AsmGen::emitCmpRegReg(regMap[reg]+1, regMap[reg], beqInsn, bb, false);
            /*
                cmp b, c    ; cmmInsn
                beq equal   ; beqInsn
                mov b, a    ; mov1
                mov c, a    ; mov2
                equal:
                mov a, b    ; mov3
            */

            // mark as protected
            protected_regs->push_back(reg);
        } // else: already protected
    }
}
