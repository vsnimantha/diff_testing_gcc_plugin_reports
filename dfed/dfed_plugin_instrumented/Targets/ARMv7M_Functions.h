/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

/*
 * Header file for the ARMv7M_Functions class, which
 * implements the pure virtual functions of the ARM_ISA class
 *
 * Contains the prototypes of the overridden functions.
 */

#ifndef TARGETS_ARMV7M_FUNCTIONS_H_
#define TARGETS_ARMV7M_FUNCTIONS_H_

#include "ArmISA_Functions.h"

class ARMv7M_Functions : public ARM_ISA{
	public:
		ARMv7M_Functions(processor_type cpu);
		~ARMv7M_Functions(){}


		void changeCBZ() override;

		rtx_insn *insertPush(vector<unsigned char> &regs, unsigned char stackPointer, rtx_insn* attachRtx, basic_block bb, bool after) override;
		rtx_insn *insertPop(vector<unsigned char> &regs, unsigned char stackPointer, rtx_insn* attachRtx, basic_block bb, bool after) override;

		// Functions to change the CBZ
		static rtx createBeq(rtx_insn* codeLabel);
		static rtx createBne(rtx_insn* codeLabel);
		static rtx_insn* findLabel(unsigned int number);
};


#endif /* TARGETS_ARMV7M_FUNCTIONS_H_ */
