/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include "SCFC.h"
#include "AsmGen.h"
#include "UpdatePoint.h"
#include "InstrType.h"

/**
 * Constructor, initializes necessary variables
 */
SCFC::SCFC(ARM_ISA* isa, unsigned int nrOfRegsToUse)
	:GeneralCFED(isa, nrOfRegsToUse){
}

/**
 * Function to calculate / assign the
 * 	- compile-time signatures for each basic block
 */
void SCFC::calcVariables(){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		unsigned int si = 0;
		edge e;
		edge_iterator ei;
		FOR_EACH_EDGE(e, ei, bb->succs){
			if (e->src == bb){
				unsigned int leftShift = (e->dest)->index - 2;
				if(leftShift < (last_basic_block_for_fn(cfun))){
					si |= 1 << leftShift;
				}
			}
		}
		this->signatures[(bb->index) - 2] = si;
	}
}

/**
 * Function to insert the necessary intra-block CFE detection instructions
 */
void SCFC::insertIntraBlockJumpDetection(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	throw "Intra-block CFE detection for SCFC not officially supported and therefore not implemented!";
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * at the beginning of each basic block
 * Inserts:
 * 	CMP r10, #<idBasicBlock>
 * 	BNE .codeLabel
 */
void SCFC::insertBegin(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachBefore){
	switch(ARM_ISA::getISAtarget(arm_cpu_option)){
		case ARMv7M:
		{
			rtx_insn* prev = AsmGen::emitCmpRegInt(regsToUse[1], idBB, attachBefore, bb, false);
			AsmGen::emitBne(codeLabel, prev, bb, true);
			break;
		}
		case ARMv6M:
		default:
		{
			AsmGen::emitBneRegInt(regsToUse[1], idBB, codeLabel, attachBefore, bb, false);
			break;
		}
	}
	
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * in the middle of each basic block
 * ARMv7-M Inserts:
 * 	AND r11, #<mask> -> mask = 1 << idBasicBlock
 * 	CMP r11, #0
 * 	BEQ .codeLabel
 * 	MOV r11, #<compileTimeSignatureBasicBlock>
 * 
 * ARMv6-M Inserts:
 *  LSR r7, #idBB
 *  AND r7, r5 -> r5 always contains #1
 *  CMP r7, r5
 *  BNE .codeLabel
 */
void SCFC::insertMiddle(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachAfter){
	//rtx_insn* prev = ;
	switch(ARM_ISA::getISAtarget(arm_cpu_option)){
		case ARMv7M:
		{
			rtx_insn* prev = attachAfter;
			if (this->nrOfOrigInstr[idBB] == 1){
				prev = AsmGen::emitAndRegInt(regsToUse[0], (1<< idBB), prev, bb, false);
			}
			else{
				prev = AsmGen::emitAndRegInt(regsToUse[0], (1<< idBB), prev, bb, true);
			}
			prev = AsmGen::emitCmpRegInt(regsToUse[0], 0, prev, bb, true);
			prev = AsmGen::emitBeq(codeLabel, prev, bb, true);
			if (!InstrType::isExitBlock(bb)){
				AsmGen::emitMovRegInt(regsToUse[0], signatures[idBB], prev, bb, true);
			}
			break;
		}
		case ARMv6M:
		default:
		{
			
			rtx_insn* prev = emitLSR(idBB, attachAfter, bb);
			prev = AsmGen::emitAndRegReg(regsToUse[0], regsToUse[2], prev, bb, true);
			prev = AsmGen::emitBneRegReg(regsToUse[0], regsToUse[2], codeLabel, prev, bb, true);
			if (!InstrType::isExitBlock(bb)){
				AsmGen::emitMovRegInt(regsToUse[0], signatures[idBB], prev, bb, true);
			}
			break;
		}
	}
	
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * at the end of each basic block
 * Inserts:
 * 	MOV r10, #<idSucessorBasicBlock> -> conditional if necessary
 */
void SCFC::insertEnd(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	int trueIdDest = -1;
	int falseIdDest = -1;

	edge e;
	edge_iterator ei;
	FOR_EACH_EDGE(e, ei, bb->succs){
		if (e->src == bb){
			int idDest = (e->dest)->index-2;
			if(idDest != (bb->index)-1){
				trueIdDest = idDest;
			}
			else{
				falseIdDest = idDest;
			}
		}
	}
	// Find last real INSN
	rtx_insn* lastInsn = UpdatePoint::lastRealINSN(bb);
	// Insert MOV statement(s)
	if(trueIdDest != -1 && falseIdDest != -1){	// Conditional Branch
		switch(ARM_ISA::getISAtarget(arm_cpu_option)){
			case ARMv7M:
			{
				rtx_code condTrue = InstrType::getCondCode(lastInsn);
				rtx_insn* trueRTX = AsmGen::emitCondMovRegInt(condTrue, regsToUse[1], trueIdDest, lastInsn, bb, false);
				rtx_code condFalse = InstrType::findContraryConditionalCode(condTrue);
				rtx_insn* falseRTX = AsmGen::emitCondMovRegInt(condFalse, regsToUse[1], falseIdDest, trueRTX, bb, true);
				break;
			}
			case ARMv6M:
			default:
			{
				AsmGen::emitMovRegInt(regsToUse[1], trueIdDest, lastInsn, bb, false);	// before branch
				AsmGen::emitMovRegInt(regsToUse[1], falseIdDest, lastInsn, bb, true);	// after branch
				break;
			}
		}
		
	}
	else if(trueIdDest == -1 && falseIdDest != -1){		// Fallthrough
		AsmGen::emitMovRegInt(regsToUse[1], falseIdDest, lastInsn, bb, true);
	}
	else if(trueIdDest != -1 && falseIdDest == -1) {		// Unconditional Branch
		AsmGen::emitMovRegInt(regsToUse[1], trueIdDest, lastInsn, bb, false);
	}
}

/**
 * Function to insert the necessary setup code at the beginning
 * of the first basic block
 * Inserts:
 * 	MOV r11, #1
 * 	MOV r10, #0
 *  MOV r4, #1 (when not ARMv7-M)
 */
void SCFC::insertSetup(){
	basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
	rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
	prev = AsmGen::emitMovRegInt(regsToUse[0], 1, prev, bb, false);
	AsmGen::emitMovRegInt(regsToUse[1], 0, prev, bb, true);
	if (ARM_ISA::getISAtarget(arm_cpu_option) != ARMv7M){
		AsmGen::emitMovRegInt(regsToUse[2], 1, prev, bb, true);
	}
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions at the beginning of the basic block
 */
void SCFC::insertSelBegin(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachBefore){
	throw "Selective implementation for SCFC not officially supported and therefore not implemented!";
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions in the middle of the basic block
 */
void SCFC::insertSelMiddle(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachAfter){
	throw "Selective implementation for SCFC not officially supported and therefore not implemented!";
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions at the end of the basic block
 */
void SCFC::insertSelEnd(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	throw "Selective implementation for SCFC not officially supported and therefore not implemented!";
}

/**
 * Emits LSR r7 #<idBB>
 * Only used when ARMv6-M
 */
rtx_insn* SCFC::emitLSR(unsigned int idBB, rtx_insn* attachRtx, basic_block bb){
	rtx regSig = gen_rtx_REG(SImode, regsToUse[0]);
	rtx constInt = gen_rtx_CONST_INT(VOIDmode, idBB);
	rtx lsr = gen_rtx_LSHIFTRT(SImode, regSig, constInt);
	rtx set = gen_movsi(regSig, lsr);
	if (this->nrOfOrigInstr[idBB] == 1){
		return emit_insn_before_noloc(set, attachRtx,bb);
	}
	else{
		return emit_insn_after_noloc(set, attachRtx,bb);
	}
}
