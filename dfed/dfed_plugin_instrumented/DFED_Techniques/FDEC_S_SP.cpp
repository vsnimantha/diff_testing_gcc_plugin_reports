/*
 * FDEC_S_SP.cpp
 *
 *  Created on: Oct 15, 2020
 *      Author: jens
 */


#include "FDEC_S_SP.h"
#include "AsmGen.h"
#include "InstrType.h"
#include "UpdatePoint.h"

#include <algorithm>

FDEC_S_SP::FDEC_S_SP(ARM_ISA* isa, map<unsigned char, unsigned char> regMap)
	:GeneralDFED(isa, regMap){
}

void FDEC_S_SP::implementDuplicationComparison(rtx_insn* codeLabel){
	implementDuplication(codeLabel);
	implementComparison(codeLabel);
	implementCallHandling(codeLabel);
}

void FDEC_S_SP::implementDuplication(rtx_insn* codeLabel){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		rtx_insn* insn;
		FOR_BB_INSNS(bb, insn){
			if(NONDEBUG_INSN_P(insn) && !JUMP_P(insn) && !CALL_P(insn) && !InstrType::isUse(insn)
				&& !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn)
				&& !InstrType::isCompare(insn) && !InstrType::isStore_SP(insn)
				&& !InstrType::isPush(insn)){
				if( !InstrType::isPop(insn) && !InstrType::isLdrFromSP(insn)
					&& !InstrType::isLoadMultipleWritebackSP(insn) && !isSPonlyOperand(insn)){
					rtx_insn* duplicate = emit_copy_of_insn_after(insn, insn);						//
					duplicate->frame_related = 0;													// Necessary since duplicate does not use stack pointer
					findAndReplaceRegisters(XEXP(duplicate,3), XEXP(duplicate,3), false);					//rtx_insn* duplicate = emit_insn_after_noloc(duplicateRtx, insn, bb);
					insn = duplicate;
				}
				else if(InstrType::isPop(insn) || InstrType::isLoadMultipleWritebackSP(insn)){
					insn = handlePopInstruction(insn, bb);
				}
				else if(InstrType::isLdrFromSP(insn)){		// a load from the stack is duplicated with a MOV
					insn = handleLdrFromSP(insn, bb);
				}
			}
			else if(InstrType::isStore_SP(insn) && !InstrType::isPush(insn)){		// treats a store to the stack the same as a store to memory
				rtx_insn* lastRealSafeInsn = UpdatePoint::lastRealSafeINSN(bb);
				rtx_insn* lastRealInsn = UpdatePoint::lastRealINSN(bb);
				if(!InstrType::isCBZ(lastRealInsn) && InstrType::isCondJump(lastRealInsn)){
					if(lastRealSafeInsnBehind(insn, lastRealInsn, lastRealSafeInsn)){
						insertMultipleComparisonInstructionsBefore(insn, codeLabel,bb);
					}
				}
				else{
					insertMultipleComparisonInstructionsBefore(insn, codeLabel,bb);
				}
			}
		}
	}
}

void FDEC_S_SP::implementComparison(rtx_insn* codeLabel){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		if(InstrType::isExitBlock(bb)){
			rtx_insn* lastInsn = UpdatePoint::lastRealINSN(bb);
			// emit CMP / BNE pair for each used register in the algorithm
			map<unsigned char, unsigned char>::const_iterator citr;
			for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
				if (citr->second > 0 && citr->first != SP_REGNUM){
					rtx_insn* cmpInsn = AsmGen::emitCmpRegReg(regMap[citr->first], citr->first, lastInsn, bb, false);
					AsmGen::emitBne(codeLabel, cmpInsn, bb, true);
				}
			}
			if(InstrType::isPop(lastInsn)){
				rtx_insn* prev = changePopreturn(lastInsn, bb);
				prev = handlePopInstruction(prev, bb);
				// emit the return
				rtx ret = gen_rtx_raw_SIMPLE_RETURN(VOIDmode);
				emit_insn_after_noloc(ret, prev, bb);
			}
		}
	}
}

void FDEC_S_SP::implementCallHandling(rtx_insn* codeLabel){
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
					insertAllCMPinstr(insn, codeLabel, bb);
				}
			}
		}
	}
}

void FDEC_S_SP::insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(origInsn, &usedRegs);							// 1) Find all used registers
	vector<unsigned char>::const_iterator citr;
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){
		if (*citr != SP_REGNUM){											// 2) For each used register, emit a CMP - BNE pair
			rtx_insn* attachRtx = AsmGen::emitCmpRegReg(*citr, regMap[*citr], origInsn, bb, false);
			AsmGen::emitBne(codeLabel, attachRtx, bb, true);
		}
	}
	// Determine whether or not an extra MOV is necessary
	if (InstrType::storeHasAUTOINCelement(origInsn)){
		rtx set = XEXP(origInsn, 3);
		rtx mem = XEXP(set,0);
		rtx autoInc = XEXP(mem, 0);
		unsigned char regNr = XINT(XEXP(autoInc, 0),0);
		origInsn = AsmGen::emitMovRegReg(regMap[regNr], regNr, origInsn, bb, true);
	}
}

rtx_insn* FDEC_S_SP::handlePopInstruction(rtx_insn* popInsn, basic_block bb){
	rtx_insn* prev = popInsn;
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(popInsn, &usedRegs);									// 1) Find all used registers
	vector<unsigned char>::const_iterator citr;
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){				// 2) Emit the necessary MOV instr.
		if (*citr != SP_REGNUM){
			prev = AsmGen::emitMovRegReg(regMap[*citr], *citr, prev, bb, true);
		}
	}
	return prev;
}

rtx_insn* FDEC_S_SP::handleLdrFromSP(rtx_insn* insn, basic_block bb){
	rtx set = XEXP(insn,3);
	rtx reg = XEXP(set,0);
	int regNr = XINT(reg,0);
	return AsmGen::emitMovRegReg(regMap[regNr], regNr, insn, bb, true);
}

void FDEC_S_SP::handleCall(rtx_insn* insn, basic_block bb){
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0 && citr->first != SP_REGNUM){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, insn, bb, false);
		}
	}
}

void FDEC_S_SP::insertAllCMPinstr(rtx_insn* callInsn, rtx_insn* codeLabel, basic_block bb){
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0 && citr->first != SP_REGNUM){
			rtx_insn* cmpInsn = AsmGen::emitCmpRegReg(regMap[citr->first], citr->first, callInsn, bb, false);
			AsmGen::emitBne(codeLabel, cmpInsn, bb, true);
		}
	}
}

rtx_insn* FDEC_S_SP::changePopreturn(rtx_insn* popInsn, basic_block bb){
	// Get current parallel and vector values
	rtx oldPar = XEXP(popInsn, 3);
	rtvec origVector = XVEC(oldPar, 0);
	unsigned char origVecLength = XVECLEN(oldPar, 0);
	// copy the necessary elements into a new vector
	rtvec newVector = rtvec_alloc(origVecLength-1); // no (return) anymore in vector
	for (int i = 1 ; i < origVecLength; i++){
		newVector->elem[i-1] = origVector->elem[i];
	}
	// create the new parallel rtx
	rtx parallel = gen_rtx_PARALLEL(VOIDmode, newVector);
	// change reg(PC) to reg(LR)
	rtx regLR = gen_rtx_REG(SImode, LR_REGNUM);
	XEXP(newVector->elem[origVecLength-2], 0) = regLR;
	// set new parallel as expression for popInsn
	XEXP(popInsn, 3) = parallel;
	// Change fields of popInsn to match new parallel
	popInsn->code = INSN;
	popInsn->u.fld[5].rt_int = -1;
	popInsn->u.fld[6].rt_rtx = NULL;
	return popInsn;
}

bool FDEC_S_SP::isSPonlyOperand(rtx_insn* insn){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(insn, &usedRegs);
	return (usedRegs.size() == 1 && usedRegs[0] == SP_REGNUM);
}

bool FDEC_S_SP::lastRealSafeInsnBehind(rtx_insn* storeInsn, rtx_insn* lastRealInsn, rtx_insn* lastRealSafeInsn){
	rtx_insn* currInsn = NEXT_INSN(storeInsn);
	while(currInsn != lastRealSafeInsn && currInsn != lastRealInsn){
		currInsn = NEXT_INSN(currInsn);
	}
	if(currInsn == lastRealSafeInsn){
		return true;
	}
	else{
		return false;
	}
}

void FDEC_S_SP::findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs){
	if(expr != 0x00){											// Validate currExpr is not null
		if(expr->code != ASM_OPERANDS){
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
						findAllUsedRegisters(subExpr, usedRegs);		// Recursive call to this function, with the sub expression
					}
					else if(format[x] == 'E'){						// Test if a Vector
						for(int i = 0; i < XVECLEN(expr, 0); i++){		// Loop over all expression in the vector
							rtx subExpr = XVECEXP(expr, 0, i);			// Get the expression
							findAllUsedRegisters(subExpr, usedRegs);	// Recursive call to this function, with the sub expression
						}
					}
				}
			}
		}
	}
}


