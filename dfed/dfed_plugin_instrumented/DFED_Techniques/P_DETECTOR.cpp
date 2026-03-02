#include "P_DETECTOR.h"

P_DETECTOR::P_DETECTOR(ARM_ISA *isa, map<unsigned char, unsigned char> regmap, unsigned char signatureReg, unsigned char ssPtr1, unsigned char parityReg, bool protectSp) : DETECTOR(isa, regmap, signatureReg, ssPtr1, parityReg, {})
{
    this->compareSubroutine = "P_DETECTOR_parity";
    this->spProtected = protectSp;
}

VulnerableSection *P_DETECTOR::createVulnerableSection(basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn* lastCmpInsn) {
    return new VsP_DETECTOR(this, bb, startInsn, vulnerableInsns, endInsn, isExitSection, prevVulnerableSection, signature, lastCmpInsn);
}

void P_DETECTOR::implementTechnique(void)
{
    errorLabel = insertError();
    DETECTOR::implementTechnique();
}
