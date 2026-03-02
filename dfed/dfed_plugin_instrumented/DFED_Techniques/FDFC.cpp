/*
 * FullDuplication.cpp
 *
 *  Created on: Mar 10, 2020
 *      Author: jens
 */


#include <FDFC.h>
#include "emit-rtl.h"
#include "InstrType.h"
#include "AsmGen.h"
#include "UpdatePoint.h"

#include <algorithm>


FDFC::FDFC(ARM_ISA* isa, map<unsigned char, unsigned char> regMap)
	: GeneralDFED(isa, regMap){
	popStrings.reserve(6);
}

void FDFC::implementDuplicationComparison(rtx_insn* codeLabel){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		rtx_insn* lastRealSafeInsn = UpdatePoint::lastRealSafeINSN(bb);
		rtx_insn* lastRealInsn = UpdatePoint::lastRealINSN(bb);
		rtx_insn* insn;
		FOR_BB_INSNS(bb, insn){
			if(NONDEBUG_INSN_P(insn) && !JUMP_P(insn) && !CALL_P(insn) && !InstrType::isUse(insn)
				&& !InstrType::isUnspecVolatile(insn) && !InstrType::isUnspec(insn)){
				if(!InstrType::isCompare(insn) && !InstrType::isStore(insn) && !InstrType::isPush(insn) && !InstrType::isPop(insn)){
					rtx_insn* duplicate = emit_copy_of_insn_after(insn, insn);						//
					duplicate->frame_related = 0;													// Necessary since duplicate does not use stack pointer
					findAndReplaceRegisters(XEXP(duplicate,3), XEXP(duplicate,3), !InstrType::isLdrSPargument(insn, this->functionArgumentsFromFifth));					//rtx_insn* duplicate = emit_insn_after_noloc(duplicateRtx, insn, bb);
					if(InstrType::isCondExec(duplicate)){	// No comparison necessary if conditional execution
						insn = duplicate;
					}
					else if (!InstrType::isCBZ(lastRealInsn) && InstrType::isCondJump(lastRealInsn)){
						// Only insert a comparison instruction if it is safe to do so
						// It is only safe if the lastRealSafeInsn is behind the current insn
						if(lastRealSafeInsnBehind(duplicate, lastRealInsn, lastRealSafeInsn)){
							insn = insertSingleComparisonAfter(insn, duplicate,codeLabel, bb);
						}
						else{
							insn = duplicate;		// Make sure FOR_BB_INSN loops correctly
						}
					}
					else{	// Always insert a comparison instruction in this case
						insn = insertSingleComparisonAfter(insn, duplicate, codeLabel, bb);
					}
				}
				else if(InstrType::isPush(insn)){
					insn = handlePushInstruction(insn, bb);
				}
				else if(InstrType::isPop(insn)){
					handlePopInstruction(insn, bb);
				}
				else if(InstrType::isCompare(insn) || InstrType::isStore(insn)){		// A compare or store instruction needs no duplicate, only CMP for its used register(s)
					insertMultipleComparisonInstructionsBefore(insn, codeLabel, bb);
				}
			}
			else if(JUMP_P(insn) && InstrType::isPop(insn)){		// POP{<reglist>, PC} must be duplicated!
				handlePopInstruction(insn, bb);
			}
		}
	}
}

bool FDFC::lastRealSafeInsnBehind(rtx_insn* duplicate, rtx_insn* lastRealInsn, rtx_insn* lastRealSafeInsn){
	rtx_insn* currInsn = NEXT_INSN(duplicate);
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

rtx_insn* FDFC::insertSingleComparisonAfter(rtx_insn* origInsn, rtx_insn* attachRtx, rtx_insn* codeLabel, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(origInsn, &usedRegs);							// 1) Find all used registers
	unsigned char firstReg = usedRegs[0];								// 2) Select the first used one
	switch(firstReg){
	case 13:															// 3a) SP and r6 do not have to match, so no CMP
		return attachRtx;
		break;
	default:
		attachRtx = AsmGen::emitCmpRegReg(firstReg, regMap[firstReg], attachRtx, bb, true);		// 3b) All others have to match, so CMP
		return AsmGen::emitBne(codeLabel, attachRtx, bb, true);
	}
}

void FDFC::insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb){
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

rtx_insn* FDFC::handlePushInstruction(rtx_insn* pushInsn, basic_block bb){
	vector<unsigned char> usedRegs;
	findAllUsedRegisters(pushInsn, &usedRegs);									// 1) Find all used registers
	vector<unsigned char>::const_iterator citr;
	static string push = "STMDB r" + to_string(regMap[SP_REGNUM]) + "!, {";		// 2) Build the PUSH string using usedRegs
	for(citr = usedRegs.begin(); citr != usedRegs.end(); citr++){
		if (*citr != SP_REGNUM && *citr != LR_REGNUM){
			push += "r" + to_string(regMap[*citr]) + ",";
		}
	}
	push.pop_back();
	push += "}";
	return AsmGen::emitAsmInput(push.c_str(), pushInsn, bb, true);				// 3) Emit and return the duplicate PUSH insn
}

void FDFC::handlePopInstruction(rtx_insn* popInsn, basic_block bb){
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

void FDFC::findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs){
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
