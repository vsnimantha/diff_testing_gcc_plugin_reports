/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <map>

#include "DFEDcreator.h"
#include "GeneralDFED.h"
// TODO Evaluate if different ISA functions are necessary
#include "ArmISA_Functions.h"
#include "ARMv6M_Functions.h"
#include "ARMv7M_Functions.h"
#include "ARMv8M_Functions.h"

#include "FDFC.h"
#include "FDEC.h"
#include "FDEC_S.h"
#include "FDSC.h"
#include "FDEC_S_SP.h"
#include "SWIFT.h"
#include "SWIFT_R.h"
#include "S_SWIFT_R.h"
#include "DETECTOR.h"
#include "P_DETECTOR.h"

/**
 * Function to implement the selected DFE detection technique.
 */
void DFEDcreator::implementTechnique(const char *technique, struct plugin_argument *args, int argc){
	// 1) Create object for the ISA
	ARM_ISA* isa;
	switch(ARM_ISA::getISAtarget(arm_cpu_option)){
		case ARMv6M:
			throw "ARMv6M is too limited to implement any available DFED technique!\n";
			break;
		case ARMv7M:
			isa = new ARMv7M_Functions(arm_cpu_option);
			break;
		case ARMv8M:
			isa = new ARMv8M_Functions(arm_cpu_option);
			break;
		default:
			throw "Not supported target and therefore unknown ISA!\n";
			break;
	}
	
	// 2) Create the map containing the register mapping
	// TODO BDB: can this be more generic? e.g. other ISA's?
	map<unsigned char, unsigned char> regMap = {
			{0,7},
			{1,8},
			{2,9},
			{3,10},
			{4,11},
			{LR_REGNUM,12},
			{SP_REGNUM, 6},
	};

	/*
	 * General decision: 
	 * Due to limitations of ARMv6-M instruction set, 
	 * DFED techniques that use duplication and comparison are not-supported for this ISA
	 */
	// 3) Create object for the DFE detection technique					 
	GeneralDFED* genDFED;
	if(!strcmp(technique, "FDFC")){ // unpublished
		genDFED = new FDFC(isa, regMap);
	} else if(!strcmp(technique, "FDEC")){ // unpublished
		genDFED = new FDEC(isa, regMap);
	} else if(!strcmp(technique, "FDEC_S")){ // unpublished
		genDFED = new FDEC_S(isa, regMap);
	} else if(!strcmp(technique, "FDSC")){ // 2018 - 10.1109/PRDC.2018.00037
		genDFED = new FDSC(isa, regMap);
	} else if(!strcmp(technique, "FDEC_S_SP")){ // unpublished
		regMap = {{0,6},{1,7},{2,8},{3,9},{4,10},{5,11},{LR_REGNUM, 12}, {SP_REGNUM, SP_REGNUM}};
		genDFED = new FDEC_S_SP(isa, regMap);
	} else if(!strcmp(technique, "SWIFT")){ // 2005 - 10.1109/CGO.2005.34
		genDFED = new SWIFT(isa, regMap);
	} else if (!strcmp(technique, "SWIFT-R")) { // 2006 - 10.1109/DSN.2006.15
		regMap = {{0,7},{1,9},{2,11},{LR_REGNUM, 3}, {SP_REGNUM,5}};
		genDFED = new SWIFT_R(isa, regMap);
	}
	else if (!strcmp(technique, "S-SWIFT-R")) {
		regMap = {{3,4}};
		
		// TODO define the regmap based on a plugin argument instead:
		// vector<char> protRegs = isa->getRegsFromStr(DFED_PLUGIN::findArgumentValue("protected_regs", args, argc));
		// regMap = createRegMapFromProtRegs(protRegs);
		// Note: if we do this, findArgumentValue should probably be in a supporting class instead of DFED_PLUGIN

		genDFED = new S_SWIFT_R(isa, regMap);
	} else if (!strcmp(technique, "DETECTOR")) { // 2021 - 10.1109/ICSRS53853.2021.9660636
		regMap = {{0,0},{1,0},{2,0},{3,0},{4,0},{7,0},{8,0},{9,0},{10,0},{12,0},{LR_REGNUM,0},{SP_REGNUM,0}};
		genDFED = new DETECTOR(isa, regMap, 11, 5, 6, {0,1,2,3,4,7,8,9,10,12,SP_REGNUM});
	} else if (!strcmp(technique, "S-DETECTOR")) {
		//! When using S-DETECTOR:
		//  - Change the protectedRegs vector to only include the registers to be protected
		//  - Don't forget to edit the S-DETECTOR_compare assembly code!
		vector<unsigned char> protectedRegs = {0, 1, 2, 3, 4, 7, 8, 9, 10, 12, SP_REGNUM}; // <- LR_REGNUM is not allowed!

		regMap = {{0,0},{1,0},{2,0},{3,0},{4,0},{7,0},{8,0},{9,0},{10,0},{12,0},{LR_REGNUM,0},{SP_REGNUM,0}};
		genDFED = new DETECTOR(isa, regMap, 11, 5, 6, protectedRegs);
	} else if (!strcmp(technique, "P-DETECTOR")) {
		regMap = {{0,0},{1,0},{2,0},{3,0},{4,0},{7,0},{8,0},{9,0},{10,0},{12,0},{LR_REGNUM,0},{SP_REGNUM,0}};
		genDFED = new P_DETECTOR(isa, regMap, 11, 5, 6);
	} else if (!strcmp(technique, "S-P-DETECTOR")) {
		//! When using S-P-DETECTOR:
		//  - Set protectSp to true or false, depending on whether the stack pointer should be protected
		//	- Don't forget to edit the S-P-DETECTOR_parity assembly code!
		const bool protectSp = false;

		regMap = {{0,0},{1,0},{2,0},{3,0},{4,0},{7,0},{8,0},{9,0},{10,0},{12,0},{LR_REGNUM,0},{SP_REGNUM,0}};
		genDFED = new P_DETECTOR(isa, regMap, 11, 5, 6, protectSp);
	} else{
		throw "Unknown technique supplied to implement!\n";	
	}

	// 4) Implement the selected technique
	genDFED->implementTechnique();
}
