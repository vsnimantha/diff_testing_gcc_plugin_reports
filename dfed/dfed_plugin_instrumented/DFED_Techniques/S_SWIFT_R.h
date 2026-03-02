/*
 * FDEC.h
 *
 *  Created on: Mar 30, 2020
 *      Author: jens
 */

#include <gcc-plugin.h>
#include <basic-block.h>
#include <rtl.h>
#include <vector>

#include "GeneralDFED.h"


class S_SWIFT_R : public GeneralDFED{
	public:
		S_SWIFT_R(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
		~S_SWIFT_R(){}

	private:
		/**
		   /**
		 * Override the pure virtual function of GeneralDFED
		 * Implements the Selective Duplication technique known as SWIFT_R
		 */
		void implementDuplicationComparison(rtx_insn* codeLabel);

        void implementDuplication(rtx_insn* codeLabel);
		void implementComparison(rtx_insn* codeLabel);
        void implementCallHandling(rtx_insn* codeLabel);

		bool selected_register(rtx_insn* origInsn, rtx subExpr, char offset);
		bool selected_register2(rtx_insn* origInsn, rtx currExpr, char offset);
		bool selected_register3(rtx_insn* origInsn, rtx currExpr, char offset);
		bool selected_register4(rtx_insn* origInsn, rtx currExpr, char offset);
        bool should_duplicate(rtx_insn* expr);
        void handleCall(rtx_insn* insn, basic_block bb);
		void findTheRegisters(rtx_insn* origInsn, rtx currExpr, char offset, vector<unsigned char>* usedRegs);
        uint8_t find_shadow_sp_regnum();
        const uint8_t SHADOW_SP = SP_REGNUM;
        rtx_insn* update_shadow_regs_after_pop(rtx_insn* insn, basic_block bb);
        void find_all_used_registers(rtx expr, vector<unsigned char>* usedRegs);
		void find_all_registers(rtx expr, vector<unsigned char>* usedRegs);
		bool checking(rtx_insn* origInsn);
        void insertSetup() override;

        bool should_insert_comparison(rtx_insn* insn);
        void insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel);
};


 /* DFED_TECHNIQUES_FDEC_H_ */
