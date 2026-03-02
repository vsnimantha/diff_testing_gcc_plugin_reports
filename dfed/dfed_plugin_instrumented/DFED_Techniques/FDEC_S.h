/*
 * FDEC_S.h
 *
 *  Created on: Mar 31, 2020
 *      Author: jens
 */

#ifndef DFED_TECHNIQUES_FDEC_S_H_
#define DFED_TECHNIQUES_FDEC_S_H_

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <vector>

#include "GeneralDFED.h"

class FDEC_S : public GeneralDFED{
	public:
		FDEC_S(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		~FDEC_S(){}

	private:
		/**
		 * Overrides the pure virtual function of GeneralDFED
		 * Implements the FullDuplication technique
		 */
		void implementDuplicationComparison(rtx_insn* codeLabel);

		void implementDuplication(rtx_insn* codeLabel);
		void implementComparison(rtx_insn* codeLabel);

		void findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs);

		void insertMultipleComparisonInstructionsBefore(rtx_insn* origInsn, rtx_insn* codeLabel, basic_block bb);
		rtx_insn* handlePushInstruction(rtx_insn* pushInsn, basic_block bb);
		void handlePopInstruction(rtx_insn* popInsn, basic_block bb);

		vector<string> popStrings;
		vector<string> pushStrings;
};



#endif /* DFED_TECHNIQUES_FDEC_S_H_ */
