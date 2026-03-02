/*
 * FDEC_S_SP.h
 *
 *  Created on: Oct 15, 2020
 *      Author: jens
 */

#ifndef DFED_TECHNIQUES_FDEC_S_SP_H_
#define DFED_TECHNIQUES_FDEC_S_SP_H_


#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <vector>

#include "GeneralDFED.h"

class FDEC_S_SP : public GeneralDFED{
	public:
		FDEC_S_SP(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		~FDEC_S_SP(){}

	private:
		/**
		 * Overrides the pure virtual function of GeneralDFED
		 * Implements the FullDuplication technique
		 */
		void implementDuplicationComparison(rtx_insn* codeLabel);

		void implementDuplication(rtx_insn* codeLabel);
		void implementComparison(rtx_insn* codeLabel);
		void implementCallHandling(rtx_insn* codeLabel);

		void findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs);

		void insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb);
		rtx_insn* handlePopInstruction(rtx_insn* popInsn, basic_block bb);
		rtx_insn* handleLdrFromSP(rtx_insn* insn, basic_block bb);
		void handleCall(rtx_insn* insn, basic_block bb);
		void insertAllCMPinstr(rtx_insn* callInsn, rtx_insn* codeLabel, basic_block bb);

		bool isSPonlyOperand(rtx_insn* insn);

		rtx_insn* changePopreturn(rtx_insn* popInsn, basic_block bb);

		bool lastRealSafeInsnBehind(rtx_insn* duplicate, rtx_insn* lastRealInsn, rtx_insn* lastRealSafeInsn);
};


#endif /* DFED_TECHNIQUES_FDEC_S_SP_H_ */
