/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include "ARMv8M_Functions.h"
#include "AsmGen.h"


ARMv8M_Functions::ARMv8M_Functions(processor_type cpu)
	:ARM_ISA(cpu) {}

vector<char> ARMv8M_Functions::getRegsFromStr(const char* regsStr) {
	throw "not supported at the moment";
}

void ARMv8M_Functions::changeCBZ(){
	throw "not supported at the moment";
}

rtx_insn *ARMv8M_Functions::insertPush(vector<unsigned char> &regs, unsigned char stackPointer, rtx_insn* attachRtx, basic_block bb, bool after){
	throw "not supported at the moment";
}

rtx_insn *ARMv8M_Functions::insertPop(vector<unsigned char> &regs, unsigned char stackPointer, rtx_insn* attachRtx, basic_block bb, bool after){
	throw "not supported at the moment";
}


