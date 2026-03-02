/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>
#include <tree.h>

#include "InstrType.h"

#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

/**
 * Method to determine whether or not the provided
 * rtx_insn is a compare instruction
 */
bool InstrType::isCompare(rtx_insn* expr){
	if(expr != 0x00 && INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		bool isPar = findCode(innerExpr, PARALLEL);
		bool isCMP = findCode(innerExpr, COMPARE);
		return (!isPar && isCMP);
	}
	else{
		return false;
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a Parallel Compare instruction
 */
bool InstrType::isParallelCompare(rtx_insn* expr){
	if(expr != 0x00 && INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		bool isPar = findCode(innerExpr, PARALLEL);
		bool isCMP = findCode(innerExpr, COMPARE);
		return (isPar && isCMP);
	}
	else{
		return false;
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a conditionally executed instruction
 */
bool InstrType::isCondExec(rtx_insn* expr){
	rtx innerExpr = XEXP(expr, 3);
	return findCode(innerExpr, COND_EXEC);
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a return instruction
 */
bool InstrType::isReturn(rtx_insn* expr){
	rtx innerExpr = XEXP(expr, 3);
	bool ret =  findCode(innerExpr, RETURN);
	bool simpRet = findCode(innerExpr, SIMPLE_RETURN);
	return (ret || simpRet);
}

/**
 * Method to determine whether or not the provided
 * basic block is an exit basic block
 */
bool InstrType::isExitBlock(basic_block bb){
	unsigned int nrOfSuccs = 0;
	int idSuccs = -5;
	edge e;
	edge_iterator ei;
	FOR_EACH_EDGE(e, ei, bb->succs){
		nrOfSuccs++;
		if(nrOfSuccs == 1){
			idSuccs = e->dest->index - 2;
		}
	}
	return ( (nrOfSuccs == 1) && (idSuccs == -1) );
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a use instruction (RTL syntax)
 */
bool InstrType::isUse(rtx_insn* expr){
    if (INSN_P(expr)){
	    rtx innerExpr = XEXP(expr, 3);
	    rtx_code exprCode = (rtx_code) innerExpr->code;
	    return (exprCode == USE);
    }
    else{
        return false;
    }
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is an unspec instruction (RTL syntax)
 */
bool InstrType::isUnspec(rtx_insn* expr){
	if(INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		rtx_code exprCode = (rtx_code) innerExpr->code;
		return (exprCode == UNSPEC);
	}
	else{
		return false;
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a clobber instruction (RTL syntax)
 */
bool InstrType::isClobber(rtx_insn* expr){
	if(INSN_P(expr)){
		rtx inner = XEXP(expr, 3);
		rtx_code innerCode = (rtx_code) inner->code;
		return (innerCode == CLOBBER);
	}
	else{
		return false;
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a volatile unspec instruction (RTL syntax)
 */
bool InstrType::isUnspecVolatile(rtx_insn* expr){
	if(INSN_P(expr)){
		rtx inner = XEXP(expr, 3);
		rtx_code innerCode = (rtx_code) inner->code;
		return (innerCode == UNSPEC_VOLATILE);
	}
	else{
		return false;
	}
}

bool InstrType::isPush(rtx_insn* expr){
	if(INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		bool isPar = findCode(innerExpr, PARALLEL);
		bool isMem = findCode(innerExpr, MEM);
		bool isPreModify = findCode(innerExpr, PRE_MODIFY);
		bool hasSPReg = findRegWithNumber(innerExpr, SP_REGNUM);
		return (isPar && isMem && isPreModify && hasSPReg);
	}
	else{
		return false;
	}
}

bool InstrType::isPop(rtx_insn* expr){
	if(INSN_P(expr) || JUMP_P(expr)){
		int insnNumber = XINT(expr,5);
		if (insnNumber != -1){
			string insnName = get_insn_name(insnNumber);
			return (insnName.find("pop") != string::npos);
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
	/*
		rtx innerExpr = XEXP(expr, 3);
		bool isPar = findCode(innerExpr, PARALLEL);
		bool isMem = findCode(innerExpr, MEM);
		bool isPreModify = findCode(innerExpr, PRE_MODIFY);
		bool hasSPReg = findRegWithNumber(innerExpr, SP_REGNUM);
		return (isPar && isMem && !isPreModify && hasSPReg);
	*/
}

bool InstrType::isLoadMultipleWritebackSP(rtx_insn* expr){
	if(INSN_P(expr)){
		int insnNumber = XINT(expr, 5);
		if (insnNumber != -1){
			string insnName = get_insn_name(insnNumber);
			bool isLoadMultipleWriteback =  (insnName.find("load_multiple_with_writeback") != string::npos);
			if (isLoadMultipleWriteback){
				rtx par = XEXP(expr, 3);
				rtvec parVec = XVEC(par,0);
				rtx setSP = parVec->elem[0];
				return (XINT(XEXP(setSP,0),0) == SP_REGNUM);
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool InstrType::isStore(rtx_insn* expr, bool allowCondExec){
	if(INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		bool processConditional = !findCode(innerExpr, COND_EXEC) || allowCondExec;
		if(processConditional && !findRegWithNumber(innerExpr, SP_REGNUM)){
			rtx set = getCode(innerExpr, SET);
			if(set != 0x00){
				rtx candidateMem = XEXP(set, 0);
				return (candidateMem->code == MEM);
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool InstrType::isStore_SP(rtx_insn* expr, bool allowCondExec){
	if(INSN_P(expr)){
		rtx innerExpr = XEXP(expr, 3);
		if(!findCode(innerExpr, COND_EXEC) || allowCondExec){
			rtx set = getCode(innerExpr, SET);
			if(set != 0x00){
				rtx candidateMem = XEXP(set, 0);
				return (candidateMem->code == MEM);
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

unsigned char InstrType::getStore_SpReg(rtx_insn* expr) {
	rtx innerExpr = XEXP(expr, 3);
	rtx set = getCode(innerExpr, SET);
	rtx reg = XEXP(set, 1);
	if(reg->code == REG)
	{
		return XINT(reg,0);
	} else {
		// printf("Could not find the register of the store to sp\n");
		return -1;
	}	
}

unsigned char InstrType::getLoad_SpReg(rtx_insn* expr) {
	rtx innerExpr = XEXP(expr, 3);
	rtx set = getCode(innerExpr, SET);
	rtx reg = XEXP(set, 1);
	if(reg->code == REG)
	{
		return XINT(reg,0);
	} else {
		// printf("Could not find the register of the ldr to sp\n");
		return -1;
	}
}

int InstrType::getStore_SpOffset(rtx_insn *expr){
	rtx innerExpr = XEXP(expr, 3);
	rtx plus = getCode(innerExpr, PLUS);
	if(plus != 0x00)
	{
		rtx cst = XEXP(plus,1);
		return XINT(cst, 0);
	} else {
		return 0;
	}
}

int InstrType::getLoad_SpOffset(rtx_insn *expr) {
	rtx innerExpr = XEXP(expr, 3);
	rtx plus = getCode(innerExpr, PLUS);
	if(plus != 0x00)
	{
		rtx cst = XEXP(plus,1);
		return XINT(cst, 0);
	} else {
		return 0;
	}
}

bool InstrType::isLdrFromSP(rtx_insn* expr){
	if (INSN_P(expr) && !JUMP_P(expr)){
		rtx set = XEXP(expr, 3);
		rtx reg = XEXP(set, 0);
		rtx mem = XEXP(set,1);

		if(reg != 0x00 && mem != 0x00 && reg->code == REG && mem->code == MEM){ 
			if (findRegWithNumber(mem, SP_REGNUM)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}else{
		return false;
	}
}

bool InstrType::isLdrSPargument(rtx_insn* expr, vector<string> arguments){
	if(INSN_P(expr)){
		rtx set = XEXP(expr, 3);
		if(XEXP(set,0)->code == REG && XEXP(set,1)->code == MEM){	// Know we have an LDR
			rtx mem = XEXP(set, 1);
			if(findRegWithNumber(mem, SP_REGNUM)){	// Know we have an LDR r<reg>, [SP]
				return memContainsArg(mem, arguments);
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

/**
 * 	TODO: 
 *  @warning InstrType::isLoad was made by student Elias Verstappe & is not yet tested
 * 	Function that checks if insn is any kind of load
 */
bool InstrType::isLoad(rtx_insn* expr) {
	if (INSN_P(expr)) {
		rtx set = XEXP(expr, 3);
		if (XEXP(set, 0)->code == REG) {
			if (findCode(XEXP(set, 1), MEM)) {				
				return true;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool InstrType::storeHasAUTOINCelement(rtx_insn* expr){
	if (isStore_SP(expr)){
		rtx set = XEXP(expr, 3);
		rtx mem = XEXP(set,0);
		rtx candidateAUTOINC = XEXP(mem, 0);
		return (GET_RTX_CLASS(candidateAUTOINC->code) == RTX_AUTOINC);
	}
	else{
		return false;
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a conditional jump instruction
 */
bool InstrType::isCondJump(rtx_insn* insn){
	if(isReturn(insn)){
		return false;
	}
	else{
		rtx set = XEXP(insn, 3);
		return (findCode(set, IF_THEN_ELSE) && JUMP_P(insn) );
	}
}

/**
 * Method to determine whether or not the provided
 * rtx_insn is a compare-branch zero instruction
 */
bool InstrType::isCBZ(rtx_insn* insn){
	rtx_insn* innerExpr = insn;//XEXP(insn, 4);
	bool isPar = findCode(innerExpr, PARALLEL);
	bool isIfTE = findCode(innerExpr, IF_THEN_ELSE);
	bool isConstIntZero = findConstIntWithNumber(innerExpr, 0);
	return (isPar && isIfTE && isConstIntZero && JUMP_P(insn));
}

/**
 * Method to determine the condition code of the
 * provided rtx_insn
 */
rtx_code InstrType::getCondCode(rtx_insn* condExpr){
	rtx set = XEXP(condExpr, 3);
	return getConditionalCode(set, IF_THEN_ELSE);
}

/**
 * Method to determine the contrary condition code
 * of the provided condition code.
 */
rtx_code InstrType::findContraryConditionalCode(enum rtx_code condition){
	switch(condition){
		case EQ:
			return NE;
			break;
		case NE:
			return EQ;
			break;
		case GT:
			return LE;
			break;
		case LE:
			return GT;
			break;
		case GTU:
			return LEU;
			break;
		case LEU:
			return GTU;
			break;
		case LT:
			return GE;
			break;
		case GE:
			return LT;
			break;
		case LTU:
			return GEU;
			break;
		case GEU:
			return LTU;
			break;
		default:
			printf("False Conditional code, returned the code\n");
			return condition;
			break;
	}
}

// ----------------------- Private Section -------------------------- \\

/**
 * Method to find the provided rtx_code in the given
 * rtx. Is a recursive method to make sure all fields of
 * the provided rtx is examined.
 */
bool InstrType::findCode(rtx expr, rtx_code code){
	if( expr == 0x00 ){
		return false;
	}

	rtx_code exprCode = (rtx_code) expr->code;			// Get the code of the expression
	const char* format = GET_RTX_FORMAT(exprCode);		// Get the format of the expression, tells what operands are expected

	if(exprCode == code){					// Test if expression is a CODE expression
		return true;
	}
	else if(exprCode == ASM_OPERANDS){
		return false;
	}
	else{
		for (int x=0; x < GET_RTX_LENGTH(exprCode); x++){	// Loop over all characters in the format
			if(format[x] == 'e'){							// Test if they are an expression
				rtx subExpr = XEXP(expr,x);					// Get the expression
				if (findCode(subExpr, code)){				// Recursive call to this function
					return true;
				}
			}
			else if(format[x] == 'E'){						// Test if a Vector
				for(int i=0; i<XVECLEN(expr,0);i++){		// Loop over all expressions in the vector
					rtx subExpr = XVECEXP(expr, 0, i);		// Get the expression
					if(findCode(subExpr, code)){			// Recursive call to this function
						return true;
					}
				}
			}
		}
	}
	return false;
}

rtx InstrType::getCode(rtx expr, rtx_code code){
	if( expr == 0x00 ){
		return 0x00;
	}

	rtx_code exprCode = (rtx_code) expr->code;			// Get the code of the expression
	const char* format = GET_RTX_FORMAT(exprCode);		// Get the format of the expression, tells what operands are expected

	if(exprCode == code){					// Test if expression is a CODE expression
		return expr;
	}
	else if(exprCode == ASM_OPERANDS){
		return 0x00;
	}
	else{
		for (int x=0; x < GET_RTX_LENGTH(exprCode); x++){	// Loop over all characters in the format
			if(format[x] == 'e'){							// Test if they are an expression
				rtx subExpr = XEXP(expr,x);					// Get the expression
				rtx found = getCode(subExpr, code);			// Recursive call to this function
				if (found != 0x00 && found->code == code){
					return found;
				}
			}
			else if(format[x] == 'E'){						// Test if a Vector
				for(int i=0; i<XVECLEN(expr,0);i++){		// Loop over all expressions in the vector
					rtx subExpr = XVECEXP(expr, 0, i);		// Get the expression
					rtx found = getCode(subExpr, code);		// Recursive call to this function
					if(found != 0x00 && found->code == code){
						return found;
					}
				}
			}
		}
	}
	return 0x00;
}

/**
 * Method to find a CONST_INT rtx with the provided number in the given
 * rtx. Is a recursive method to make sure all fields of
 * the provided rtx is examined.
 */
bool InstrType::findConstIntWithNumber(rtx expr, unsigned int number){
	if( expr == 0x00){
		return false;
	}
	rtx_code exprCode = (rtx_code) expr->code;			// Get the code of the expression
	const char* format = GET_RTX_FORMAT(exprCode);		// Get the format of the expression, tells what operands are expected

	if(exprCode == CONST_INT){								// Test if expression is a Const_Int expression
		if(XINT(expr,0) == number){
			return true;
		}
	}
	else if(exprCode == ASM_OPERANDS){
			return false;
		}
	else{
		for (int x=0; x < GET_RTX_LENGTH(exprCode); x++){	// Loop over all characters in the format
			if(format[x] == 'e'){							// Test if they are an expression
				rtx subExpr = XEXP(expr,x);					// Get the expression
				if (findConstIntWithNumber(subExpr, number)){				// Recursive call to this function
					return true;
				}
			}
			else if(format[x] == 'E'){						// Test if a Vector
				for(int i=0; i<XVECLEN(expr,0);i++){		// Loop over all expressions in the vector
					rtx subExpr = XVECEXP(expr, 0, i);		// Get the expression
					if(findConstIntWithNumber(subExpr, number)){			// Recursive call to this function
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool InstrType::findRegWithNumber(rtx expr, unsigned char number){
	if( expr == 0x00){
		return false;
	}
	rtx_code exprCode = (rtx_code) expr->code;			// Get the code of the expression
	const char* format = GET_RTX_FORMAT(exprCode);		// Get the format of the expression, tells what operands are expected

	if(exprCode == REG){								// Test if expression is a Const_Int expression
		if(XINT(expr,0) == number){
			return true;
		}
	}
	else if(exprCode == ASM_OPERANDS){
			return false;
		}
	else{
		for (int x=0; x < GET_RTX_LENGTH(exprCode); x++){	// Loop over all characters in the format
			if(format[x] == 'e'){							// Test if they are an expression
				rtx subExpr = XEXP(expr,x);					// Get the expression
				if (findRegWithNumber(subExpr, number)){				// Recursive call to this function
					return true;
				}
			}
			else if(format[x] == 'E'){						// Test if a Vector
				for(int i=0; i<XVECLEN(expr,0);i++){		// Loop over all expressions in the vector
					rtx subExpr = XVECEXP(expr, 0, i);		// Get the expression
					if(findRegWithNumber(subExpr, number)){			// Recursive call to this function
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
 * Method to find the provided conditional rtx_code in the given
 * rtx. Is a recursive method to make sure all fields of
 * the provided rtx is examined.
 */
rtx_code InstrType::getConditionalCode(rtx expr, rtx_code code){
	if( expr == 0x00){
			return (rtx_code)-1;
		}
		rtx_code exprCode = (rtx_code) expr->code;			// Get the code of the expression
		const char* format = GET_RTX_FORMAT(exprCode);		// Get the format of the expression, tells what operands are expected

		if(exprCode == code){
			rtx cond = XEXP(expr, 0);
			return GET_CODE(cond);// Test if expression is a CODE expression
		}
		else{
			for (int x=0; x < GET_RTX_LENGTH(exprCode); x++){	// Loop over all characters in the format
				if(format[x] == 'e'){							// Test if they are an expression
					rtx subExpr = XEXP(expr,x);					// Get the expression
					rtx_code condCode = getConditionalCode(subExpr, code);
					if (condCode!= -1){				// Recursive call to this function
						return condCode;
					}
				}
				else if(format[x] == 'E'){						// Test if a Vector
					for(int i=0; i<XVECLEN(expr,0);i++){		// Loop over all expressions in the vector
						rtx subExpr = XVECEXP(expr, 0, i);		// Get the expression
						rtx_code condCode = getConditionalCode(subExpr, code);
						if(condCode != -1){			// Recursive call to this function
							return condCode;
						}
					}
				}
			}
		}
		return (rtx_code)-1;
}

bool InstrType::memContainsArg(rtx mem, vector<string> arguments){
	bool found = false;
	tree memExprTree = CONST_CAST_TREE(MEM_EXPR(mem));
	if(DECL_NAME(memExprTree) && IDENTIFIER_POINTER(DECL_NAME(memExprTree))){
		string argName = IDENTIFIER_POINTER(DECL_NAME(memExprTree));
		vector<string>::const_iterator citr;
		for(citr = arguments.begin(); citr != arguments.end(); citr++){
			if(!argName.compare(*citr)){
				found = true;
			}
		}
	}
	return found;
}

std::string InstrType::insnType(rtx_insn* insn) {

	if (isCompare(insn)) {
		return "isCompare(insn)";
	} else if (isCondExec(insn)) {
		return "isCondExec(insn)";
	} else if (isReturn(insn)) {
		return "isReturn(insn)";
	} else if (isUse(insn)) {
		return "isUse(insn)";
	} else if (isUnspec(insn)) {
		return "isUnspec(insn)";
	} else if (isClobber(insn)) {
		return "isClobber(insn)";
	} else if (isUnspecVolatile(insn)) {
		return "isUnspecVolatile(insn)";
	} else if (isUnspecVolatile(insn)) {
		return "isUnspecVolatile(insn)";
	} else if (isPush(insn)) {
		return "isPush(insn)";
	} else if (isPop(insn)) {
		return "isPop(insn)";
	} else if (isLoadMultipleWritebackSP(insn)) {
		return "isLoadMultipleWritebackSP(insn)";
	} else if (isStore(insn)) {
		return "isStore(insn)";
	} else if (isStore_SP(insn)) {
		return "isStore_SP(insn)";
	} else if (isLdrFromSP(insn)) {
		return "isLdrFromSP(insn)";
	} else if (storeHasAUTOINCelement(insn)) {
		return "storeHasAUTOINCelement(insn)";
	} else if (isCondJump(insn)) {
		return "isCondJump(insn)";
	} else if (isCBZ(insn)) {
		return "isCBZ(insn)";
	} else if (isParallelCompare(insn)) {
		return "isParallelCompare(insn)";
	} else {
		return "Unknown insn";
	}
}

void InstrType::printInsn(rtx_insn* insn) {

	// std::cout << insnType(insn);

	if (isCompare(insn)) {
		printf("isCompare(insn)\n");
	} else if (isCondExec(insn)) {
		printf("isCondExec(insn)\n");
	} else if (isReturn(insn)) {
		printf("isReturn(insn)\n");
	} else if (isUse(insn)) {
		printf("isUse(insn)\n");
	} else if (isUnspec(insn)) {
		printf("isUnspec(insn)\n");
	} else if (isClobber(insn)) {
		printf("isClobber(insn)\n");
	} else if (isUnspecVolatile(insn)) {
		printf("isUnspecVolatile(insn)\n");
	} else if (isUnspecVolatile(insn)) {
		printf("isUnspecVolatile(insn)\n");
	} else if (isPush(insn)) {
		printf("isPush(insn)\n");
	} else if (isPop(insn)) {
		printf("isPop(insn)\n");
	} else if (isLoadMultipleWritebackSP(insn)) {
		printf("isLoadMultipleWritebackSP(insn)\n");
	} else if (isStore(insn)) {
		printf("isStore(insn)\n");
	} else if (isStore_SP(insn)) {
		printf("isStore_SP(insn)\n");
	} else if (isLdrFromSP(insn)) {
		printf("isLdrFromSP(insn)\n");
	} else if (storeHasAUTOINCelement(insn)) {
		printf("storeHasAUTOINCelement(insn)\n");
	} else if (isCondJump(insn)) {
		printf("isCondJump(insn)\n");
	} else if (isCBZ(insn)) {
		printf("isCBZ(insn)\n");
	} else if (isParallelCompare(insn)) {
		printf("isParallelCompare(insn)\n");
	} else {
		printf("Unknown insn\n");
	}
}