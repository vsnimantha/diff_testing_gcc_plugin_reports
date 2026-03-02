/*
 * FDEC_S.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: jens
 */


#include "S_SWIFT_R.h"

#include "AsmGen.h"
#include "InstrType.h"
#include "UpdatePoint.h"
#include <algorithm>

#include <iostream>
#include <stdio.h>
#include <vector>>

S_SWIFT_R::S_SWIFT_R(ARM_ISA* isa, map<unsigned char, unsigned char> regMap) : 
    GeneralDFED(isa, regMap),
    SHADOW_SP(find_shadow_sp_regnum())
{ 
}

void S_SWIFT_R::implementDuplicationComparison(rtx_insn* codeLabel) {
    implementDuplication(codeLabel);
    implementComparison(codeLabel);
    implementCallHandling(codeLabel);
}


void S_SWIFT_R::implementDuplication(rtx_insn* codeLabel) {
    
    basic_block bb;
    
    FOR_EACH_BB_FN(bb, cfun) {
        
		rtx_insn* insn;

        FOR_BB_INSNS(bb, insn) {
		
            if( should_duplicate(insn) && checking(insn)) {
				
				FILE* fp = fopen("myOutput.txt", "a");
				print_rtl_single(fp, insn);
				fclose(fp);
				//if(){
				printf("first part done \n");
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
				//}
				
            } else if ( InstrType::isPop(insn) ) {

                if ( !InstrType::isReturn(insn) ) { // If the pc is popped, it is considered a return
                    // Copy over values from original pop to shadow registers
                    insn = update_shadow_regs_after_pop(insn, bb);
                }
            
            } else if ( InstrType::isPush(insn) ) {
					
				// FIXME what does this do?
                // Update shadow SP
                if(SHADOW_SP == SP_REGNUM)
                {
                    //insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, insn, bb, true);                   

                }
                else
                {
                    insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, insn, bb, true);
                    insn = AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, insn, bb, true);
                }
            }
        }
	}
}

void S_SWIFT_R::implementComparison(rtx_insn* codeLabel) {
    basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
        rtx_insn* insn;
        uint position = 0;
        
        rtx_insn* cmpInsn = nullptr;
        uint cmpPos = 0;

        // A map of unsafe areas: in these areas no cmp instructions should be inserted
        std::map<uint, uint> unsafeAreas; // <uint start, uint end>

        // A map with the instructions before which the cmp/bne should be injected
        std::map<uint, rtx_insn*> addCmpBeforeInsn; // <uint pos, rtx_insn* instruction>

        // A map with the critical instructions
        std::map<uint, rtx_insn*> criticalInsn; // <uint pos, rtx_insn* instruction>

        // bool duplicatedCmp = false;
        char duplicatedCmp = 0; // false
        bool alreadyProtected = false; 
        
        /********************************************************************
         * 1. MAKE A LIST OF THE UNSAFE AREAS AND THE CRITICAL INSTRUCTIONS *
         ********************************************************************/
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
                            unsafeAreas.insert(std::pair<uint,uint>(cmpPos, position));             // add unsafe area
                            addCmpBeforeInsn.insert(std::pair<uint,rtx_insn*>(cmpPos, cmpInsn));    // add cmp instruction
                            criticalInsn.insert(std::pair<uint,rtx_insn*>(cmpPos, insn));           // add critical instruction

                        } else {
                            // We have a conditional execution / jump without a condition: This should not happen!
                            printf("\x1b[91m[WARNING]: No cmp found for instruction %u.\n\x1b[0m", position);
                        }

                    } else { // only the first instruction in a "critical block" should be protected
                        // For non-conditional instructions:
                        // We don't really have an unsafe area, so start == end

                        unsafeAreas.insert(std::pair<uint,uint>(position, position));             // add unsafe area
                        addCmpBeforeInsn.insert(std::pair<uint,rtx_insn*>(position, insn));       // add critical instruction
                        criticalInsn.insert(std::pair<uint,rtx_insn*>(position, insn));           // |
                        
                    }
                }
                position++;
            }
        }

        /**********************************************
         * 2. INSERT THE CMP/BNE INSTRUCTIONS         *
         * ! ASSUMING THAT THE ITERATION IS ORDERED   *
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

bool S_SWIFT_R::selected_register(rtx_insn* origInsn, rtx currExpr, char offset) {
	/*rtx set = XEXP(origInsn, 3);  	
	if(XEXP(set, 0)->code == REG)
	{
		unsigned char currRegNr = XINT(currExpr, 0);
		printf("currRegNr %u\n",currRegNr);
		return (regMap.count(currRegNr) != 0);
	}
	return 1 ;*/

	if(currExpr != 0x00){											// Validate currExpr is not null
		rtx_code exprCode = (rtx_code) currExpr->code;
		if(exprCode != ASM_OPERANDS){
			if(REG_P(currExpr)){										// Test if currExpr is a REG rtx	
				unsigned char currRegNr = XINT(currExpr, 0);
				printf("%u\n",currRegNr);
				return (regMap.count(currRegNr) != 0);
			}
			else{
				const char* format = GET_RTX_FORMAT(currExpr->code);
				for(int x = 0; x < GET_RTX_LENGTH(currExpr->code); x++){	// Loop over all characters in the format
					if(format[x] == 'e'){									// Test if current character indicates an expression
						rtx subExpr = XEXP(currExpr, x);					// Get the expression
						selected_register(origInsn, subExpr, offset);			// Recursive call to this function, with the sub expression
					}
					else if(format[x] == 'E'){								// Test if a Vector
						for(int i = 0; i < XVECLEN(currExpr, 0); i++){		// Loop over all expression in the vector
							rtx subExpr = XVECEXP(currExpr, 0, i);			// Get the expression
							selected_register(origInsn, subExpr, offset);		// Recursive call to this function, with the sub expression
						}
					}
				}
			}
		}
	}

  

}
bool S_SWIFT_R::selected_register2(rtx_insn* origInsn, rtx currExpr, char offset) {
	  

	if (INSN_P(origInsn)){
		rtx set = XEXP(origInsn, 3);
		if(XEXP(set, 0)->code == REG)
		{
			rtx reg = XEXP(set, 0);
			printf("reg\n",reg);
				//rtx_code exprCode = (rtx_code) currExpr->code;
				 unsigned char currRegNr = XINT(reg, 0);
					printf("%u\n",currRegNr);
				
		
			return (regMap.count(currRegNr) != 0);

		}
		return 0;
	}
	
}

bool S_SWIFT_R::selected_register3(rtx_insn* origInsn, rtx currExpr, char offset) {

	
	printf("Trusttttttttttttttt\n");
	if(currExpr != 0x00){
		printf("hiiiiiiiiiuh\n");
		
		rtx_code exprCode = (rtx_code) currExpr->code;
		/*
			if(REG_P(currExpr)){
					unsigned char currRegNr = XINT(currExpr, 0);
					printf("%u\n",currRegNr);
					if(regMap.count(currRegNr) != 0 ){
						printf("Here\n");
						return 1;
					}
			}
	*/
			if (REG_P(currExpr) && regMap.count(XINT(currExpr, 0)) != 0){
				printf("FOUND %u\n",XINT(currExpr, 0));
				return true;
			}
			else {
				printf("hiiiiiiiiiiiii12345\n");
				const char* format = GET_RTX_FORMAT(currExpr->code);
				for(int x = 0; x < GET_RTX_LENGTH(currExpr->code); x++){	// Loop over all characters in the format
					if(format[x] == 'e'){									// Test if current character indicates an expression
						rtx subExpr = XEXP(currExpr, x);					// Get the expression
						if( selected_register3(origInsn, subExpr, offset)	){
							return true;
						}		// Recursive call to this function, with the sub expression
					}
					else if(format[x] == 'E'){								// Test if a Vector
						for(int i = 0; i < XVECLEN(currExpr, 0); i++){		// Loop over all expression in the vector
					 		rtx subExpr = XVECEXP(currExpr, 0, i);			// Get the expression
							if( selected_register3(origInsn, subExpr, offset) ){
								return true;
							}		// Recursive call to this function, with the sub expression
						}
					}
				}
			}
	}
	printf("HERE");
	return false;  
		
}

void S_SWIFT_R::findTheRegisters(rtx_insn* origInsn, rtx currExpr, char offset, vector<unsigned char>* usedRegs) {

	printf("WooooHooooo");
	if(currExpr != 0x00){
		if(REG_P(currExpr)){
			unsigned char candidateRegNr = XINT(currExpr, 0);
			if(regMap.count(candidateRegNr) && find(usedRegs->begin(), usedRegs->end(),candidateRegNr) == usedRegs->end()){
				usedRegs->push_back(candidateRegNr);
			}		
		}
		else{
			const char* format = GET_RTX_FORMAT(currExpr->code);
			for(int x = 0; x < GET_RTX_LENGTH(currExpr->code); x++){	// Loop over all characters in the format
				if(format[x] == 'e'){								// Test if current character indicates an expression
						rtx subExpr = XEXP(currExpr, x);					// Get the expression
						findTheRegisters(origInsn, subExpr, offset, usedRegs);		// Recursive call to this function, with the sub expression
				}
				else if(format[x] == 'E'){						// Test if a Vector
						for(int i = 0; i < XVECLEN(currExpr, 0); i++){		// Loop over all expression in the vector
							rtx subExpr = XVECEXP(currExpr, 0, i);			// Get the expression
							findTheRegisters(origInsn, subExpr, offset, usedRegs);	// Recursive call to this function, with the sub expression
						}
				}
			}

		}

	}

}



bool S_SWIFT_R::selected_register4(rtx_insn* origInsn, rtx currExpr, char offset) {
	
	int i=0;
	currExpr = XEXP(origInsn, i);  
	while(currExpr != 0x00)			// Validate currExpr is not null
	{
		rtx_code exprCode = (rtx_code) currExpr->code;
		if(XEXP(currExpr, 0)->code == REG) 
		{
			rtx reg = XEXP(currExpr, 0);
			printf("reg\n",reg);
			unsigned char currRegNr = XINT(reg, 0);
			return (regMap.count(currRegNr) != 0);
		}
		else {
			i++;
			currExpr = XEXP(origInsn, i);  

		}
	}
	return 0;   
	
}

bool S_SWIFT_R::should_duplicate(rtx_insn* expr) {
    
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

void S_SWIFT_R::insertSetup(){
    basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
	rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0 && citr->first != SP_REGNUM){
            printf("CUSTOM SETUP");
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, prev, bb, false);
			AsmGen::emitMovRegReg(regMap[citr->first]+1, citr->first, prev, bb, false);
		}
	}
    // Sync the sp as well
    if(SHADOW_SP == SP_REGNUM)
    {
        //AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, prev, bb, false);                  
    }
    else{
        AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, prev, bb, false);
        AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, prev, bb, false);
    }
}


uint8_t S_SWIFT_R::find_shadow_sp_regnum() {
    for(auto entry = usedRegMap.begin(); entry != usedRegMap.end(); ++entry) {
        if (entry->first == SP_REGNUM) {
            return regMap[entry->first];
        }
    }
    return SP_REGNUM;
}

rtx_insn* S_SWIFT_R::update_shadow_regs_after_pop(rtx_insn* pop_insn, basic_block bb) {
    vector<unsigned char> usedRegs;
	find_all_used_registers(pop_insn, &usedRegs);
    
    // For every register that is popped
    for(auto &reg : usedRegs) {
        // Insert MOV <shadow reg> <original reg>, after pop_insn;
        pop_insn = AsmGen::emitMovRegReg(regMap[reg], reg, pop_insn, bb, true);
        pop_insn = AsmGen::emitMovRegReg(regMap[reg]+1, reg, pop_insn, bb, true);
    }
    if(SHADOW_SP == SP_REGNUM)
    {
       // pop_insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, pop_insn, bb, true);
    }
    else
    {
        pop_insn = AsmGen::emitMovRegReg(SHADOW_SP, SP_REGNUM, pop_insn, bb, true);
        pop_insn = AsmGen::emitMovRegReg(SHADOW_SP+1, SP_REGNUM, pop_insn, bb, true);
    }
    
    return pop_insn;
}


void S_SWIFT_R::find_all_used_registers(rtx expr, vector<unsigned char>* usedRegs) {
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

void S_SWIFT_R::find_all_registers(rtx expr, vector<unsigned char>* usedRegs) {
	if(expr != 0x00){											// Validate currExpr is not null
		if(REG_P(expr)){					// Test if currExpr is a REG rtx
			unsigned char candidateRegNr = XINT(expr, 0);
			if(candidateRegNr != CC_REGNUM && find(usedRegs->begin(), usedRegs->end(),candidateRegNr) == usedRegs->end()){
				usedRegs->push_back(candidateRegNr);
			}
		}
		else{
			const char* format = GET_RTX_FORMAT(expr->code);
			for(int x = 0; x < GET_RTX_LENGTH(expr->code); x++){	// Loop over all characters in the format
				if(format[x] == 'e'){								// Test if current character indicates an expression
					rtx subExpr = XEXP(expr, x);					// Get the expression
					find_all_registers(subExpr, usedRegs);		// Recursive call to this function, with the sub expression
				}
				else if(format[x] == 'E'){						// Test if a Vector
					for(int i = 0; i < XVECLEN(expr, 0); i++){		// Loop over all expression in the vector
						rtx subExpr = XVECEXP(expr, 0, i);			// Get the expression
						find_all_registers(subExpr, usedRegs);	// Recursive call to this function, with the sub expression
					}
				}
			}
		}
	}
}

bool S_SWIFT_R::should_insert_comparison(rtx_insn* insn) {
    
    return (
        JUMP_P(insn) ||
        CALL_P(insn) ||
        InstrType::isStore(insn) ||
        InstrType::isStore_SP(insn) ||
        InstrType::isPush(insn)
    );
}

void S_SWIFT_R::insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel) {
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

void S_SWIFT_R::implementCallHandling(rtx_insn* codeLabel){
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

void S_SWIFT_R::handleCall(rtx_insn* insn, basic_block bb){
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, insn, bb, false);
			AsmGen::emitMovRegReg(regMap[citr->first]+1, citr->first, insn, bb, false);
		}
	}
}

bool S_SWIFT_R::checking(rtx_insn* origInsn){
	vector<unsigned char> usedRegs;
	vector<unsigned char>::const_iterator citr;
	find_all_registers(origInsn, &usedRegs);
	
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++ ){
		printf("RegMAP contains r%d\n", *citr);
	}
	printf("Testing \n");
	unsigned char firstReg = usedRegs[0];
	printf("FIRST REG %u\n",firstReg);
	return regMap.count(firstReg);
	
}

