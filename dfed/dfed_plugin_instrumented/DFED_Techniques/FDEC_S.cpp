/*
 * FDEC_S.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: jens
 */


#include "FDEC_S.h"
#include "AsmGen.h"
#include "InstrType.h"
#include "UpdatePoint.h"

#include <algorithm>

FDEC_S::FDEC_S(ARM_ISA* isa, map<unsigned char, unsigned char> regMap)
	:GeneralDFED(isa, regMap){
	popStrings.reserve(6);
	pushStrings.reserve(6);
}

void FDEC_S::implementDuplicationComparison(rtx_insn* codeLabel){
	implementDuplication(codeLabel);
	implementComparison(codeLabel);
}

void FDEC_S::implementDuplication(rtx_insn* codeLabel){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		rtx_insn* insn;
		FOR_BB_INSNS(bb, insn){
			if(NONDEBUG_INSN_P(insn) && !JUMP_P(insn) && !CALL_P(insn) && !InstrType::isUse(insn)
				&& !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn)
				&& !InstrType::isCompare(insn) && !InstrType::isStore(insn)){
				if( !InstrType::isPush(insn) && !InstrType::isPop(insn)){
					rtx_insn* duplicate = emit_copy_of_insn_after(insn, insn);						//
					duplicate->frame_related = 0;													// Necessary since duplicate does not use stack pointer
					bool isFunctionArgument = InstrType::isLdrSPargument(insn, this->functionArgumentsFromFifth);
					findAndReplaceRegisters(XEXP(duplicate,3), XEXP(duplicate,3), !isFunctionArgument);					//rtx_insn* duplicate = emit_insn_after_noloc(duplicateRtx, insn, bb);
					insn = duplicate;
				}
				else if(InstrType::isPush(insn)){
					insn = handlePushInstruction(insn, bb);
				}
				else if(InstrType::isPop(insn)){
					handlePopInstruction(insn, bb);
				}
			}
			else if(InstrType::isStore(insn)){
				insertMultipleComparisonInstructionsBefore(insn, codeLabel,bb);
			}
			else if(JUMP_P(insn) && InstrType::isPop(insn)){		// POP{<reglist>, PC} must be duplicated!
				handlePopInstruction(insn, bb);
			}
		}
	}
}

void FDEC_S::implementComparison(rtx_insn* codeLabel){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		if(InstrType::isExitBlock(bb)){
			rtx_insn* lastInsn = UpdatePoint::lastRealINSN(bb);
			// if instruction is POP, then a duplicate has already been inserted, so lastInsn is the duplicate
			if(InstrType::isPop(lastInsn)){
				lastInsn = PREV_INSN(lastInsn);
			}
			// emit CMP / BNE pair for each used register in the algorithm
			map<unsigned char, unsigned char>::const_iterator citr;
			for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
				if (citr->second > 0 && citr->first != SP_REGNUM){
					rtx_insn* cmpInsn = AsmGen::emitCmpRegReg(regMap[citr->first], citr->first, lastInsn, bb, false);
					AsmGen::emitBne(codeLabel, cmpInsn, bb, true);
				}
			}
		}
	}
}

void FDEC_S::insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(origInsn, &usedRegs);							// 1) Find all used registers
	vector<unsigned char>::const_iterator citr;
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){
		if (*citr != SP_REGNUM){											// 2) For each used register, emit a CMP - BNE pair
			rtx_insn* attachRtx = AsmGen::emitCmpRegReg(*citr, regMap[*citr], origInsn, bb, false);
			AsmGen::emitBne(codeLabel, attachRtx, bb, true);
		}
	}
}

rtx_insn* FDEC_S::handlePushInstruction(rtx_insn* pushInsn, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(pushInsn, &usedRegs);									// 1) Find all used registers
	if (usedRegs.size() == 2 && usedRegs[1] == LR_REGNUM){						// Not necessary when only LR is being pushed
		return pushInsn;														// usedRegs[0] = SP, usedRegs[1] = LR
	}
	else{
		vector<unsigned char>::const_iterator citr;
		string push = "STMDB r" + to_string(regMap[SP_REGNUM]) + "!, {";		// 2) Build the PUSH string using usedRegs
		for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){
			if (*citr != SP_REGNUM && *citr != LR_REGNUM){
				push += "r" + to_string(regMap[*citr]) + ",";
			}
		}
		push.pop_back();
		push += "}";
		pushStrings.push_back(push);
		return AsmGen::emitAsmInput((pushStrings[pushStrings.size()-1]).c_str(), pushInsn, bb, true);				// 3) Emit and return the duplicate PUSH insn
	}
}

void FDEC_S::handlePopInstruction(rtx_insn* popInsn, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(popInsn, &usedRegs);									// 1) Find all used registers
	vector<unsigned char>::const_iterator citr;
	string pop = "LDMIA r" + to_string(regMap[SP_REGNUM]) + "!, {";
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){				// 2) Build the POP string using usedRegs
		if (*citr != SP_REGNUM && *citr != LR_REGNUM){
			pop += "r" + to_string(regMap[*citr]) + ",";
		}
	}
	pop.pop_back();
	pop += "}";
	popStrings.push_back(pop);
	AsmGen::emitAsmInput((popStrings[popStrings.size()-1]).c_str(), popInsn, bb, false);	// 3) Emit and return the duplicate PUSH insn
}

void FDEC_S::findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs){
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

