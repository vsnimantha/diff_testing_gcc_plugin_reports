/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>
#include <tree.h>

#include <stdio.h>

#include "GeneralDFED.h"
#include "AsmGen.h"
#include "UpdatePoint.h"

/**
 * Constructor, initializes the necessary variables.
 */
GeneralDFED::GeneralDFED(ARM_ISA* isa, map<unsigned char, unsigned char> regMap){
	this->regMap = regMap; 		// Should be a copy
	this->usedRegMap = regMap;	// Should be a copy
	this->isa = isa;
	map<unsigned char, unsigned char>::iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		citr->second = 0;		// Initialize the values to 0
	}
	this->insnID = get_max_uid();
	srand(time(NULL));
	this->functionArgumentsFromFifth = getArgumentsVector();
}

/**
 * Function which implements the selected CFE detection technique.
 * Determines in which order the pure virtual functions are executed.
 */
void GeneralDFED::implementTechnique(){
	// 1) Insert the jump the DFED_Detected
	// printf("1) Insert the jump the DFED_Detected\n");
	rtx_insn* codeLabel = insertError();

	// 2) Implement the technique
	// printf("2) Implement the technique\n");
	implementDuplicationComparison(codeLabel);

	// 3) Insert the setup code
	// printf("3) Insert the setup code\n");
	insertSetup();
}

/**
 * Function that emits a code label at the end of the current function
 * and emits the call to the CFED_Detected error handler.
 */
rtx_insn* GeneralDFED::insertError(){
	rtx_insn* prev = get_last_insn();
	basic_block bb = BASIC_BLOCK_FOR_FN(cfun,last_basic_block_for_fn(cfun)-1);
	rtx_insn* codeLabel = AsmGen::emitCodeLabel(insnID++, prev, bb, true);
	AsmGen::emitCall(codeLabel);
	return codeLabel;
}

void GeneralDFED::insertSetup(){
	basic_block bb = BASIC_BLOCK_FOR_FN(cfun, 2);
	rtx_insn* prev = UpdatePoint::firstRealINSN(bb);
	map<unsigned char, unsigned char>::const_iterator citr;
	for(citr = usedRegMap.begin(); citr != usedRegMap.end(); citr++){
		if (citr->second > 0 && citr->first != SP_REGNUM){
			AsmGen::emitMovRegReg(regMap[citr->first], citr->first, prev, bb, false);
		}
	}
}

void GeneralDFED::findAndReplaceRegisters(rtx origInsn, rtx currExpr, bool replaceSP, char offset){
	if(currExpr != 0x00){											// Validate currExpr is not null
		rtx_code exprCode = (rtx_code) currExpr->code;
		if(exprCode != ASM_OPERANDS){
			if(REG_P(currExpr)){										// Test if currExpr is a REG rtx
				replaceRegisters(origInsn, currExpr, replaceSP, offset); // Replace the register numbers
			}
			else{
				const char* format = GET_RTX_FORMAT(currExpr->code);
				for(int x = 0; x < GET_RTX_LENGTH(currExpr->code); x++){	// Loop over all characters in the format
					if(format[x] == 'e'){									// Test if current character indicates an expression
						rtx subExpr = XEXP(currExpr, x);					// Get the expression
						findAndReplaceRegisters(origInsn, subExpr, replaceSP, offset);			// Recursive call to this function, with the sub expression
					}
					else if(format[x] == 'E'){								// Test if a Vector
						for(int i = 0; i < XVECLEN(currExpr, 0); i++){		// Loop over all expression in the vector
							rtx subExpr = XVECEXP(currExpr, 0, i);			// Get the expression
							findAndReplaceRegisters(origInsn, subExpr, replaceSP, offset);		// Recursive call to this function, with the sub expression
						}
					}
				}
			}
		}
	}
}

vector<unsigned char> GeneralDFED::getUsedRegisters(rtx insn, rtx currExpr, vector<unsigned char> &foundRegs){
	if(currExpr != 0x00){											// Validate currExpr is not null
		rtx_code exprCode = (rtx_code) currExpr->code;
		if(exprCode != ASM_OPERANDS){
			if(REG_P(currExpr)){										// Test if currExpr is a REG rtx
				foundRegs.push_back(XINT(currExpr, 0));
				return foundRegs;
			}
			else{
				const char* format = GET_RTX_FORMAT(currExpr->code);
				for(int x = 0; x < GET_RTX_LENGTH(currExpr->code); x++){	// Loop over all characters in the format
					if(format[x] == 'e'){									// Test if current character indicates an expression
						rtx subExpr = XEXP(currExpr, x);					// Get the expression
						getUsedRegisters(insn, subExpr, foundRegs);					// Recursive call to this function, with the sub expression
					}
					else if(format[x] == 'E'){								// Test if a Vector
						for(int i = 0; i < XVECLEN(currExpr, 0); i++){		// Loop over all expression in the vector
							rtx subExpr = XVECEXP(currExpr, 0, i);			// Get the expression
							getUsedRegisters(insn, subExpr, foundRegs);				// Recursive call to this function, with the sub expression
						}
					}
				}
			}
		}
	}
}

void GeneralDFED::replaceRegisters(rtx origInsn, rtx regExpr, bool replaceSP, char offset){
	unsigned char currRegNr = XINT(regExpr, 0);				// Get the register number
	bool replace = false;									// Determine whether or not to replace the regnumber
	switch(currRegNr){
		case SP_REGNUM:
			replace = replaceSP;
			break;
		default:
			replace = (regMap.count(currRegNr) != 0);
			break;
	}
	if (replace){												// Replace the register number if allowed
		rtx newReg = gen_rtx_REG(SImode, regMap[currRegNr]+offset);	// create a new REG rtx, with the number counter part
		replace_rtx(origInsn, regExpr, newReg);					// replace the original REG rtx with the nex REG rtx
		usedRegMap[currRegNr] += 1;								// Update the used register map
	}

}

vector<string> GeneralDFED::getArgumentsVector(){
	vector<string> arguments;
	char argCount = 0;
	for(tree arg = DECL_ARGUMENTS(current_function_decl); arg; arg = DECL_CHAIN(arg)){
		argCount++;
		if (argCount > 4){
			string argument = IDENTIFIER_POINTER(DECL_NAME(arg));
			arguments.push_back(argument);
		}
	}
	return arguments;
}


void GeneralDFED::implementDuplicationComparison(rtx_insn* codeLabel) {
	throw "GeneralDFED::implementDuplicationComparison(rtx_insn* codeLabel) should be overridden";
}