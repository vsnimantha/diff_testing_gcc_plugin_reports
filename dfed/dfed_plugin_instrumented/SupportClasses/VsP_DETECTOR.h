#ifndef VsP_DETECTOR_H
#define VsP_DETECTOR_H

#include "VulnerableSection.h"

class VsP_DETECTOR : public VulnerableSection
{

public:
    VsP_DETECTOR(P_DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn *lastCmpInsn);

    rtx_insn* prependInsn(bool alternativeImplementation) override;
    rtx_insn* appendInsn(bool alternativeImplementation) override;
    rtx_insn* insertStart(bool alternativeImplementation) override;
};
#endif