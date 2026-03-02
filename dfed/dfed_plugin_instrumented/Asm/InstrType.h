/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

/*
 * This is the header file for the InstrType class.
 *
 * It contains the headers of the methods used to determine
 * whether or not the provided rtx_insn represents a certain instruction
 */

#ifndef ASM_INSTRTYPE_H_
#define ASM_INSTRTYPE_H_

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <vector>
#include <string>

using namespace std;

class InstrType{
	public:
		static bool isCompare(rtx_insn* expr);
		static bool isCondExec(rtx_insn* expr);
		static bool isReturn(rtx_insn* expr);
		static bool isExitBlock(basic_block bb);
		static bool isUse(rtx_insn* expr);
		static bool isUnspec(rtx_insn* expr);
		static bool isClobber(rtx_insn* expr);
		static bool isUnspecVolatile(rtx_insn* expr);
		static bool isPush(rtx_insn* expr);
		static bool isPop(rtx_insn* expr);
		static bool isLoadMultipleWritebackSP(rtx_insn* expr);
		static bool isStore(rtx_insn* expr, bool allowCondExec = false);
		static bool isStore_SP(rtx_insn* expr, bool allowCondExec = false);
		static bool isLdrFromSP(rtx_insn* expr);
		static bool isLdrSPargument(rtx_insn* expr, vector<string> arguments);
		static bool isLoad(rtx_insn* expr);

		static bool storeHasAUTOINCelement(rtx_insn* expr);

		static bool isCondJump(rtx_insn* insn);
		static rtx_code getCondCode(rtx_insn* condExpr);
		static rtx_code findContraryConditionalCode(enum rtx_code condition);

		static bool isCBZ(rtx_insn* insn);

		static std::string insnType(rtx_insn* insn);
		static void printInsn(rtx_insn* insn);
		static bool isParallelCompare(rtx_insn* expr);
		static unsigned char getStore_SpReg(rtx_insn* expr);
		static unsigned char getLoad_SpReg(rtx_insn* expr);
		static int getStore_SpOffset(rtx_insn *expr);
		static int getLoad_SpOffset(rtx_insn *expr);
		
	private:
		static bool findCode(rtx expr, rtx_code code);
		static rtx getCode(rtx expr, rtx_code code);
		static bool findConstIntWithNumber(rtx expr, unsigned int number);
		static bool findRegWithNumber(rtx expr, unsigned char number);
		static rtx_code getConditionalCode(rtx expr, rtx_code cond);

		static bool memContainsArg(rtx mem, vector<string> arguments);
};



#endif /* ASM_INSTRTYPE_H_ */
