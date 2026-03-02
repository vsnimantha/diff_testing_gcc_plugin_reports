/*
*
*   March 2, 2021
*
*/
#ifndef P_DETECTOR_H
#define P_DETECTOR_H

#include "DETECTOR.h"
#include "VsP_DETECTOR.h"

class P_DETECTOR : public DETECTOR
{
public:
    // FIXME the  regmap is just for the GeneralDFED inheritance (and the usedRegMap initialisation)
    P_DETECTOR(ARM_ISA* isa, map<unsigned char, unsigned char> regmap, unsigned char signatureReg, unsigned char parityReg, unsigned char ssPtr, bool protectSp=true);
    ~P_DETECTOR() {}

    /**
     * Entry point called by the DFEDcreator class
     */
    void implementTechnique(void) override;

private:
    VulnerableSection *createVulnerableSection(basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn* lastCmpInsn) override;

};

#endif