/*
 * This GCC Plugin has been developed during a research grant from the Baekeland program of the Flemish Agency for Innovation and Entrepreneurship (VLAIO) in cooperation with Televic Healthcare NV, under grant agreement IWT 150696.
 * Copyright (c) 2019 Jens Vankeirsbilck & KU Leuven LRD & Televic Healthcare NV.
 * Distributed under the MIT "Expat" License. (See accompanying file LICENSE.txt)
 */

/*
 * Header file for the GeneralCFED class
 *
 * Contains the prototype of teh implementTechnique which effectively
 * implements the selected CFE detection technique.
 *
 * Contains pure virtual functions of the methods that must be implemented
 * by each supported CFE detection technique (= interface)
 */

#ifndef DFED_TECHNIQUES_GENERALDFED_H_
#define DFED_TECHNIQUES_GENERALDFED_H_

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>

#include <map>
#include <string>
#include <vector>

#include "ArmISA_Functions.h"

using namespace std;

class GeneralDFED{
	public:
		GeneralDFED(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		virtual ~GeneralDFED(){}

		/**
		 * Method to execute all necessary methods to implement a DFE detection technique
		 */
		virtual void implementTechnique();

		vector<unsigned char> getUsedRegisters(rtx insn, rtx currExpr, vector<unsigned char> &foundRegs);

	protected:
		// Copy of the provided regMap, so that each technique operates on its own copy
		map<unsigned char, unsigned char> regMap;

		map<unsigned char, unsigned char> usedRegMap;

		vector<string> functionArgumentsFromFifth;
		ARM_ISA* isa;

		/**
		 * Function to replace all REG rtx with the new numbers, based on regMap
		 */
		void findAndReplaceRegisters(rtx origInsn, rtx currExpr, bool replaceSP, char offset=0);

		/**
		 * Function to insert the setup functionality needed for the specific technique
		 */
		virtual void insertSetup();

		/**
		 * Map to count which original registers have been used, and which not.
		 * Will be used during insertSetup to only export the necessary instructions.
		 */


		/**
		 * Method to effectively implement the DFE detection technique
		 */
		virtual void implementDuplicationComparison(rtx_insn* codeLabel);

		/**
		 * Function to insert the call to the DFED_Handler function to indicate a DFE was detected
		 * returns the created codeLabel
		 */
		rtx_insn* insertError();

		/**
		 * Function doing the actual register replacement
		 */
		void replaceRegisters(rtx origInsn, rtx regExpr, bool replaceSP, char offset=0);

		vector<string> getArgumentsVector();

		unsigned int insnID;

};


#endif /* DFED_TECHNIQUES_GENERALDFED_H_ */
