/*
 * SWIFT.h
 *
 *  Created on: Feb 24, 2021
 *      Author: Elias Verstappe
 */

#ifndef DFED_TECHNIQUES_SWIFT_H_
#define DFED_TECHNIQUES_SWIFT_H_

#include "AsmGen.h"
#include "InstrType.h"
#include "UpdatePoint.h"
#include "GeneralDFED.h"
#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>

class SWIFT : public GeneralDFED {

    public:
        SWIFT(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
        ~SWIFT(){}
    
    protected:
		void implementDuplicationComparison(rtx_insn* codeLabel) override;

        virtual void implementDuplication(rtx_insn* codeLabel);
		void implementComparison(rtx_insn* codeLabel);
        void implementCallHandling(rtx_insn* codeLabel);

        virtual void listUnsafeAreasAndCriticalInstructions(basic_block bb, std::map<uint, uint>* unsafeAreas, std::map<uint, rtx_insn*>* addCmpBeforeInsn, std::map<uint, rtx_insn*>* criticalInsn);

        bool should_duplicate(rtx_insn* expr);
        virtual void handleCall(rtx_insn* insn, basic_block bb);

        uint8_t find_shadow_sp_regnum();
        const uint8_t SHADOW_SP = SP_REGNUM;

        virtual rtx_insn* update_shadow_regs_after_pop(rtx_insn* insn, basic_block bb);
        void find_all_used_registers(rtx expr, vector<unsigned char>* usedRegs);

        virtual void insertSetup() override;

        bool should_insert_comparison(rtx_insn* insn);
        virtual void insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel);

};



#endif