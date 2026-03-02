#ifndef VsDETECTOR_H
#define VsDETECTOR_H

#include "VulnerableSection.h"

class VsDETECTOR : public VulnerableSection
{

public:
    VsDETECTOR(DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn *lastCmpInsn);
    
    rtx_insn* prependInsn(bool alternativeImplementation) override;
    rtx_insn* appendInsn(bool alternativeImplementation) override;
    rtx_insn* insertStart(bool alternativeImplementation) override;
};
#endif