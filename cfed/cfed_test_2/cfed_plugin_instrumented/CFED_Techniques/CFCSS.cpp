/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include "CFCSS.h"
#include "AsmGen.h"
#include "UpdatePoint.h"
#include "InstrType.h"

/**
 * Constructor, initializes necessary variables
 */
CFCSS::CFCSS(ARM_ISA* isa, unsigned int nrOfRegsToUse)
	:GeneralCFED(isa, nrOfRegsToUse){
	diffSigs.reserve(n_basic_blocks_for_fn(cfun)-2);
	createPaths();
}

/**
 * Function to calculate / assign the
 * 	- compile-time signatures for each basic block
 * 	- differential signature for each basic block
 */
void CFCSS::calcVariables(){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		unsigned int idBB = (bb->index) - 2;
		// ToDo: this only works when less than 8 basic blocks when ARMv6M
		signatures[idBB] = 1 << idBB;
	}
	FOR_EACH_BB_FN(bb, cfun){
		calcDiffSigs(bb);
	}
}

/**
 * Function to insert the necessary intra-block CFE detection instructions
 */
void CFCSS::insertIntraBlockJumpDetection(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	throw "Intra-block CFE detection for CFCSS not officially supported and therefore not implemented!";
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * at the beginning of each basic block
 * Inserts:
 * 	EOR r11, #<differentialSignature>
 * 	EOR r11, r10 (if necessary)
 * 	CMP r11, #<compileTimeSignature>
 * 	BNE .codelabel
 */
void CFCSS::insertBegin(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachBefore){
	unsigned int inEdges = countIncomingEdges(bb);
	rtx_insn* prev;
	switch(ARM_ISA::getISAtarget(arm_cpu_option)){
		case ARMv7M:
			prev = AsmGen::emitEorRegInt(regsToUse[0], diffSigs[idBB], attachBefore, bb, false);
			if(inEdges > 1){
				prev = AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, true);
			}
			prev = AsmGen::emitCmpRegInt(regsToUse[0], signatures[idBB], prev, bb, true);
			AsmGen::emitBne(codeLabel, prev, bb, true);
			break;
		case ARMv6M:
		default:
			bool after = false;
			prev = attachBefore;
			if(inEdges > 1){
				prev = AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, after);			// EOR r7, r5 at the start (if necessary and update prev!)
				after = true;	// update after!!
			}	
			prev = AsmGen::emitMovRegInt(regsToUse[1], diffSigs[idBB], prev, bb, after);			// MOV r5, #<differentialSignature>
			rtx_insn* eor = AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, true);		// EOR r7, r5 -> EOR r7, #<differentialSignature>
			if (n_basic_blocks_for_fn(cfun)-2 > 8){
				prev = AsmGen::emitMovRegInt(regsToUse[1], signatures[idBB], eor, bb, true);
				AsmGen::emitBneRegReg(regsToUse[0], regsToUse[1], codeLabel, prev, bb, true);
			} 
			else{
				AsmGen::emitBneRegInt(regsToUse[0], signatures[idBB], codeLabel, eor, bb, true);		// CMP r7, #<compileTimeSignature> + BNE .codeLabel	
			}
			break;
	}
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * in the middle of each basic block
 */
void CFCSS::insertMiddle(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachAfter){
	// Nothing to do for CFCSS
}

/**
 * Function to insert the necessary inter-block CFE detection instructions
 * at the end of each basic block
 * Inserts
 * 	MOV r10, #<number> -> updates r10 to the correct value depending on the situation
 */
void CFCSS::insertEnd(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	if (!InstrType::isExitBlock(bb)){
		vector<CFCSSpath>::const_iterator it;
		rtx_insn* lastInsn = UpdatePoint::lastRealINSN(bb);
		if(InstrType::isCondJump(lastInsn)){
			rtx_code trueCode = InstrType::getCondCode(lastInsn);
			rtx_code falseCode = InstrType::findContraryConditionalCode(trueCode);
			unsigned int trueUpD = 0;
			unsigned int falseUpD = 0;
			// Find correct UpD values
			for(it = paths.begin(); it != paths.end(); it++){
				if((*it).startBBId == idBB && (*it).endBBId != (idBB+1)){
					trueUpD = (*it).upD;
				}
				else if((*it).startBBId == idBB){
					falseUpD = (*it).upD;
				}
			}
			if (trueUpD == falseUpD){
				AsmGen::emitMovRegInt(regsToUse[1], trueUpD, lastInsn, bb, false);
			}
			else{
				// Use UpD values to insert update
				switch(ARM_ISA::getISAtarget(arm_cpu_option)){
	    			case ARMv7M:
		    			{
		    				rtx_insn* trueRtx = AsmGen::emitCondMovRegInt(trueCode, regsToUse[1], trueUpD, lastInsn, bb, false);
							AsmGen::emitCondMovRegInt(falseCode, regsToUse[1], falseUpD, trueRtx, bb, true);
							break;
		    			}
					case ARMv6M:
		    		default:
		    			{
		    				AsmGen::emitMovRegInt(regsToUse[1], trueUpD, lastInsn, bb, false);	// insert update before branch
		    				AsmGen::emitMovRegInt(regsToUse[1], falseUpD, lastInsn,bb, true);	// insert update after branch
		    				break;
						}
				}
			}
		}
		else{
			unsigned int UpD = 0;
			for(it = paths.begin(); it != paths.end(); it++){
				if((*it).startBBId == idBB){
					UpD = (*it).upD;
				}
			}
			AsmGen::emitMovRegInt(regsToUse[1], UpD, lastInsn, bb, false);
		}
	}
}

/**
 * Function to insert the necessary setup code at the beginning of the first basic block
 * Inserts
 * 	MOV r11, #0
 * 	MOV r10, #0
 */
void CFCSS::insertSetup(){
	basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
	rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
	prev = AsmGen::emitMovRegInt(regsToUse[0], 0, prev, bb, false);
	AsmGen::emitMovRegInt(regsToUse[1], 0, prev, bb, true);
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions at the beginning of the basic block
 */
void CFCSS::insertSelBegin(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachBefore){
	//throw "Selective implementation for CFCSS not officially supported and therefore not implemented!";
	unsigned int inEdges = countIncomingEdges(bb);
	rtx_insn* prev;
	switch(ARM_ISA::getISAtarget(arm_cpu_option)){
		case ARMv7M:
			prev = AsmGen::emitEorRegInt(regsToUse[0], diffSigs[idBB], attachBefore, bb, false);
			if(inEdges > 1){
				prev = AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, true);
			}
			if(InstrType::isExitBlock(bb)){
				prev = AsmGen::emitCmpRegInt(regsToUse[0], signatures[idBB], prev, bb, true);
				AsmGen::emitBne(codeLabel, prev, bb, true);
			}
			break;
		case ARMv6M:
		default:
			if(inEdges > 1){
				AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, false);					// EOR r11, r10 at the start (if necessary)
			}
			prev = AsmGen::emitMovRegInt(regsToUse[1], diffSigs[idBB], attachBefore, bb, false);	// MOV r10, #<differentialSignature>
			rtx_insn* eor = AsmGen::emitEorRegReg(regsToUse[0], regsToUse[1], prev, bb, true);		// EOR r11, r10 -> EOR r11, #<differentialSignature>
			if(InstrType::isExitBlock(bb)){
				AsmGen::emitBneRegInt(regsToUse[0], signatures[idBB], codeLabel, eor, bb, true);			// CMP r11, #<compileTimeSignature> + BNE .codeLabel
			}			
			break;
	}
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions in the middle of the basic block
 */
void CFCSS::insertSelMiddle(unsigned int idBB, basic_block bb, rtx_insn* codeLabel, rtx_insn* attachAfter){
	//throw "Selective implementation for CFCSS not officially supported and therefore not implemented!";
	// Nothing to do for selective CFCSS
}

/**
 * Function to insert the necessary selective form of the inter-block
 * CFE detection instructions at the end of the basic block
 */
void CFCSS::insertSelEnd(unsigned int idBB, basic_block bb, rtx_insn* codeLabel){
	//throw "Selective implementation for CFCSS not officially supported and therefore not implemented!";
	insertEnd(idBB, bb, codeLabel);
}

/**
 * Function to calculate the differential signature of the basic block
 */
void CFCSS::calcDiffSigs(basic_block bb){
	int firstId = -1;
	int secondId = -1;
	unsigned int idBB = bb->index - 2;
	if(idBB != 0){
		vector<unsigned int> predIds;
		// get the ids of the sources of the incoming edges
		edge e;
		edge_iterator ei;
		FOR_EACH_EDGE(e, ei, bb->preds){
			unsigned int predId = (e->src)->index - 2;
			predIds.push_back(predId);
		}
		// check for circular loopback to block
		for (int i = 1; i < predIds.size(); i ++){
			if(predIds[i] == idBB){
				unsigned int temp = predIds[i];
				predIds[i] = predIds[0];
				predIds[0] = temp;
			}
		}
		// calculate the diffSigs and upDs
		diffSigs[idBB] = signatures[idBB] ^ signatures[predIds[0]];
		if (predIds.size() > 1){
			for( int x = 1; x < predIds.size(); x++){
				unsigned int idPred = predIds[x];
				vector<CFCSSpath>::iterator it;
				for(it = paths.begin(); it != paths.end(); it++){
					if((*it).startBBId == idPred && (*it).endBBId == idBB){
						unsigned int upD = signatures[idPred] ^ signatures[predIds[0]];
						(*it).upD = upD;
					}
				}
			}
		}
	}
	else{
		diffSigs[0] = signatures[0];
	}
}

/**
 * Function to calculate the number of incoming edges
 * of the basic block
 */
unsigned int CFCSS::countIncomingEdges(basic_block bb){
	unsigned int incomingEdges = 0;
	edge e;
	edge_iterator ei;
	FOR_EACH_EDGE(e, ei, bb->preds){
		incomingEdges++;
	}
	return incomingEdges;
}

/**
 * Creates a vector containing all different paths in the CFG
 * A path contains a start basic block and an end basic block.
 * Necessary to calculate the correct differential signatures and to update
 * r10 at the end of each basic block
 */
void CFCSS::createPaths(){
	basic_block bb;
	FOR_EACH_BB_FN(bb, cfun){
		unsigned int idBB = (bb->index) - 2;
		edge e;
		edge_iterator ei;
		FOR_EACH_EDGE(e, ei, bb->succs){
			unsigned int idSuccs = (e->dest)->index -2;
			CFCSSpath path;
			path.startBBId = idBB;
			path.endBBId = idSuccs;
			paths.push_back(path);
		}
	}
}
