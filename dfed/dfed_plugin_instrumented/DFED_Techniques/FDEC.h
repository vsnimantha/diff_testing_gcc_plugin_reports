/*
 * FDEC.h
 *
 *  Created on: Mar 30, 2020
 *      Author: jens
 */

#ifndef DFED_TECHNIQUES_FDEC_H_
#define DFED_TECHNIQUES_FDEC_H_

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <vector>

#include "GeneralDFED.h"

class FDEC : public GeneralDFED{
	public:
		FDEC(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		~FDEC(){}

	private:
		/**
		 * Overrides the pure virtual function of GeneralDFED
		 * Implements the FullDuplication technique
		 */
		void implementDuplicationComparison(rtx_insn* codeLabel);

		void implementDuplication();
		void implementComparison(rtx_insn* codeLabel);

		void findAllUsedRegisters(rtx expr, vector<unsigned char>* usedRegs);

		rtx_insn* handlePushInstruction(rtx_insn* pushInsn, basic_block bb);
		void handlePopInstruction(rtx_insn* popInsn, basic_block bb);

		vector<string> popStrings;
};


#endif /* DFED_TECHNIQUES_FDEC_H_ */
