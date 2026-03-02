#include "SWIFT.h"


SWIFT::SWIFT(ARM_ISA* isa, map<unsigned char, unsigned char> regMap) : 
    GeneralDFED(isa, regMap),
    SHADOW_SP(find_shadow_sp_regnum())
{ 
}

void SWIFT::implementDuplicationComparison(rtx_insn* codeLabel) {
    implementDuplication(codeLabel);
    implementComparison(codeLabel);
    implementCallHandling(codeLabel);
}

void SWIFT::implementDuplication(rtx_insn* codeLabel) {
    
    basic_block bb;
    
    FOR_EACH_BB_FN(bb, cfun) {
        
		rtx_insn* insn;

        FOR_BB_INSNS(bb, insn) {

            if( should_duplicate(insn) ) {
                rtx_insn* duplicate = emit_copy_of_insn_after(insn, insn);  // Make a duplicate instruction, replace registers further down
                duplicate->frame_related = 0;                               // Necessary since duplicate does not use stack pointer
                                
                //  Replace registers for duplicate instruction, based on regMap
                bool isFunctionArgument = InstrType::isLdrSPargument(insn, this->functionArgumentsFromFifth);
                findAndReplaceRegisters(XEXP(duplicate,3), XEXP(duplicate,3), !isFunctionArgument);
                                                
                insn = duplicate;

            } else if ( InstrType::isPop(insn) ) {

                if ( !InstrType::isReturn(insn) ) { // If the pc is popped, it is considered a return
                    // Copy over values from original pop to shadow registers
                    insn = update_shadow_regs_after_pop(insn, bb);
                }
            
            } else if ( InstrType::isPush(insn) ) {

                // Update shadow SP
                insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, insn, bb, true);

            }
        }
	}
}

void SWIFT::listUnsafeAreasAndCriticalInstructions(basic_block bb, std::map<uint, uint>* unsafeAreas, std::map<uint, rtx_insn*>* addCmpBeforeInsn, std::map<uint, rtx_insn*>* criticalInsn) {
    rtx_insn* insn;
    uint position = 0;
    
    rtx_insn* cmpInsn = nullptr;
    uint cmpPos = 0;
    bool duplicatedCmp = false;
    bool alreadyProtected = false; 
    FOR_BB_INSNS(bb, insn) {
        // Skip instructions dat don't matter
        if(NONDEBUG_INSN_P(insn) && !InstrType::isUse(insn) && !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn)) {

            // remember the last instruction were the flag is updated
            if (InstrType::isCompare(insn) || InstrType::isParallelCompare(insn) || InstrType::isCBZ(insn)){ // TODO BDB: What if isCBNZ?

                if (!duplicatedCmp) { // Don't add cmp/bne instructions between a comparison and it's duplicate
                    cmpInsn =  insn;        // remember the last instruction were the flag is updated ..
                    cmpPos = position;      // .. and remember its position within the BB
                }
                duplicatedCmp = (!duplicatedCmp && should_duplicate(insn)); // Mark the next instruction as a duplicated cmp if it is duplicated
            }

            // save the unsafe areas & the critical instructions
            if (should_insert_comparison(insn)) {
                if (InstrType::isCondExec(insn) || InstrType::isCondJump(insn)) {
                    // For conditional critical instructions:
                    // No cmp/bne instructions should be inserted between the last cmp and the critical instruction
                    // unsafeArea = van cmp instruction tot critical instruction

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

void SWIFT::implementComparison(rtx_insn* codeLabel) {
    basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
        // A map of unsafe areas: in these areas no cmp instructions should be inserted
        std::map<uint, uint> unsafeAreas; // <uint start, uint end>

        // A map with the instructions before which the cmp/bne should be injected
        std::map<uint, rtx_insn*> addCmpBeforeInsn; // <uint pos, rtx_insn* instruction>

        // A map with the critical instructions
        std::map<uint, rtx_insn*> criticalInsn; // <uint pos, rtx_insn* instruction>
        
        /********************************************************************
         * 1. MAKE A LIST OF THE UNSAFE AREAS AND THE CRITICAL INSTRUCTIONS *
         ********************************************************************/
        listUnsafeAreasAndCriticalInstructions(bb, &unsafeAreas, &addCmpBeforeInsn, &criticalInsn);

        /**********************************************
         * 2. INSERT THE CMP/BNE INSTRUCTIONS         *
         **********************************************/
        map<uint, uint>::const_iterator iter;
        int maxEnd = -1;
        uint lastAddedCmpBnePos = -2;
        vector<unsigned char> protectedRegs;
        for(iter = unsafeAreas.begin(); iter != unsafeAreas.end(); iter++){
            
            if((int)iter->first > maxEnd){
                // Outside unsafe area -> cmp/bne can be added at start of critical section
                maxEnd = iter->second;
                
                if(iter->first == lastAddedCmpBnePos+1) {
                    // inside a critical section
                    // add the cmp/bne at the front of the critical section
                    insert_cmp_bne_block_for_critical_insn(addCmpBeforeInsn[lastAddedCmpBnePos], criticalInsn[iter->first], &protectedRegs, bb, codeLabel);
                } else {
                    // start of a critical section
                    protectedRegs.clear();  // no registers are protected yet:
                    insert_cmp_bne_block_for_critical_insn(addCmpBeforeInsn[iter->first], criticalInsn[iter->first], &protectedRegs, bb, codeLabel);
                    lastAddedCmpBnePos = iter->first;
                }
                // emit CMP / BNE pair for each used register in the algorithm
                
                
                
            } else {
                // Inside of unsafe area -> insert at last safe point (lastAddedCmpBnePos)
                insert_cmp_bne_block_for_critical_insn(addCmpBeforeInsn[lastAddedCmpBnePos], criticalInsn[iter->first], &protectedRegs, bb, codeLabel);
            }
        }
	}
}

// If instruction is ..., return false to indicate that instructions should not be duplicated
bool SWIFT::should_duplicate(rtx_insn* expr) {
    
    return !(
        // Instructions that should NOT be duplicated:
        !NONDEBUG_INSN_P(expr) ||
        JUMP_P(expr) ||
        CALL_P(expr) ||
        InstrType::isUse(expr) ||
        InstrType::isUnspecVolatile(expr) ||
        InstrType::isUnspec(expr) ||
        InstrType::isStore(expr) ||
        InstrType::isStore_SP(expr) ||
        InstrType::isPush(expr) ||
        InstrType::isPop(expr) ||
        InstrType::isCBZ(expr) ||    
        InstrType::isCondJump(expr) ||
        InstrType::isReturn(expr) ||
        InstrType::isCompare(expr)
    );
}

void SWIFT::insertSetup(){
    GeneralDFED::insertSetup();
    // Sync the sp as well
    basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
    rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
    AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, prev, bb, false);
}


uint8_t SWIFT::find_shadow_sp_regnum() {
    for(auto entry = usedRegMap.begin(); entry != usedRegMap.end(); ++entry) {
        if (entry->first == SP_REGNUM) {
            return regMap[entry->first];
        }
    }
    return SP_REGNUM;
}

rtx_insn* SWIFT::update_shadow_regs_after_pop(rtx_insn* pop_insn, basic_block bb) {
    vector<unsigned char> usedRegs;
	find_all_used_registers(pop_insn, &usedRegs);
    
    // For every register that is popped
    for(auto &reg : usedRegs) {
        // Insert MOV <shadow reg> <original reg>, after pop_insn;
        pop_insn = AsmGen::emitMovRegReg(regMap[reg], reg, pop_insn, bb, true);
    }

    pop_insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, pop_insn, bb, true);

    return pop_insn;
}

void SWIFT::find_all_used_registers(rtx expr, vector<unsigned char>* usedRegs) {
	if(expr != 0x00){											// Validate currExpr is not null
		if(REG_P(expr)){					// Test if currExpr is a REG rtx
			unsigned char candidateRegNr = XINT(expr, 0);
			if(regMap.count(candidateRegNr) && find(usedRegs->begin(), usedRegs->end(),candidateRegNr) == usedRegs->end()){
				usedRegs->push_back(candidateRegNr);
			}
		}
		else{
			const char* format = GET_RTX_FORMAT(expr->code);
			for(int x = 0; x < GET_RTX_LENGTH(expr->code); x++){	// Loop over all characters in the format
				if(format[x] == 'e'){								// Test if current character indicates an expression
					rtx subExpr = XEXP(expr, x);					// Get the expression
					find_all_used_registers(subExpr, usedRegs);		// Recursive call to this function, with the sub expression
				}
				else if(format[x] == 'E'){						// Test if a Vector
					for(int i = 0; i < XVECLEN(expr, 0); i++){		// Loop over all expression in the vector
						rtx subExpr = XVECEXP(expr, 0, i);			// Get the expression
						find_all_used_registers(subExpr, usedRegs);	// Recursive call to this function, with the sub expression
					}
				}
			}
		}
	}
}

bool SWIFT::should_insert_comparison(rtx_insn* insn) {
    
    return (
        JUMP_P(insn) ||
        CALL_P(insn) ||
        InstrType::isStore(insn) ||
        InstrType::isStore_SP(insn) ||
        InstrType::isPush(insn) // TODO - this is only because we don't use a memory stack, so redundancy is lost after push
    );
}

void SWIFT::insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel) {
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
        if(reg != SP_REGNUM) {
            if (!std::count(protected_regs->begin(), protected_regs->end(), reg)){
                // protect this register
                
                rtx_insn* cmpInsn = AsmGen::emitCmpRegReg(regMap[reg], reg, beforeInsn, bb, false);
                AsmGen::emitBne(codeLabel, cmpInsn, bb, true);

                // mark as protected
                protected_regs->push_back(reg);
            } // else: already protected
        } else {
            spIsUsed = true;
        }
    }
    if(spIsUsed) {
        // emit CMP / BNE pair for the sp
        // TODO: adding a cmp with the sp does not work for some reason. We temporarily use a workaround.
        // mov the sp to r5
        beforeInsn = AsmGen::emitMovRegReg(5, SP_REGNUM, beforeInsn, bb, false);
        // compare r5 to the SHADOW_SP
        beforeInsn = AsmGen::emitCmpRegReg(5, SHADOW_SP, beforeInsn, bb, true);
        // emit the BNE
        beforeInsn = AsmGen::emitBne(codeLabel, beforeInsn, bb, true);
    }
}

void SWIFT::implementCallHandling(rtx_insn* codeLabel){
	bool shouldHandleCall = false;
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		rtx_insn* insn;
		FOR_BB_INSNS(bb, insn){
			if( NONDEBUG_INSN_P(insn) && !InstrType::isUse(insn)
			&& !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn) ){
				if(shouldHandleCall){
					handleCall(insn, bb);
					shouldHandleCall = false;
				}
				if ( CALL_P(insn) ){
					shouldHandleCall = true;
				}
			}
		}
	}
}

void SWIFT::handleCall(rtx_insn* insn, basic_block bb){
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, insn, bb, false);
		}
	}
}