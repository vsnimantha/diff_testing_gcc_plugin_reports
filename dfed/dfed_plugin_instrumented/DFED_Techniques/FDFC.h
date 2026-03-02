/*
 * fullDuplication.h
 *
 *  Created on: Mar 10, 2020
 *      Author: jens
 */

#ifndef DFED_TECHNIQUES_FDFC_H_
#define DFED_TECHNIQUES_FDFC_H_

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>
#include <vector>

#include "GeneralDFED.h"

class FDFC : public GeneralDFED{
	public:
		FDFC(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		~FDFC(){}

	private:
		/**
		 * Overrides the pure virtual function of GeneralDFED
		 * Implements the FullDuplication technique
		 */
		void implementDuplicationComparison(rtx_insn* codeLabel);

		bool lastRealSafeInsnBehind(rtx_insn* duplicate, rtx_insn* lastRealInsn, rtx_insn* lastRealSafeInsn);
		rtx_insn* insertSingleComparisonAfter(rtx_insn* origInsn, rtx_insn* attachRtx, rtx_insn* codeLabel, basic_block bb);
		void findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs);

		void insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb);
		rtx_insn* handlePushInstruction(rtx_insn* pushInsn, basic_block bb);
		void handlePopInstruction(rtx_insn* popInsn, basic_block bb);

		vector<string> popStrings;

};



#endif /* DFED_TECHNIQUES_FDFC_H_ */
