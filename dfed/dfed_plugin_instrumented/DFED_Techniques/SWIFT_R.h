#ifndef SWIFT_R_H
#define SWIFT_R_H

#include "SWIFT.h"

class SWIFT_R: public SWIFT {
public:
    SWIFT_R(ARM_ISA* isa, map<unsigned char, unsigned char> regMap);
    ~SWIFT_R(){}
    
private:
    void implementDuplication(rtx_insn* codeLabel) override;
    void listUnsafeAreasAndCriticalInstructions(basic_block bb, std::map<uint, uint>* unsafeAreas, std::map<uint, rtx_insn*>* addCmpBeforeInsn, std::map<uint, rtx_insn*>* criticalInsn) override;
    void handleCall(rtx_insn* insn, basic_block bb) override;
    rtx_insn* update_shadow_regs_after_pop(rtx_insn* insn, basic_block bb) override;
    void insertSetup() override;
    void insert_cmp_bne_block_for_critical_insn(rtx_insn* beforeInsn, rtx_insn * criticalInsn, vector<unsigned char> * protected_regs, basic_block bb, rtx_insn* codeLabel) override;
};

#endif