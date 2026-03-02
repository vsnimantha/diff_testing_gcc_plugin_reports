#include "VsP_DETECTOR.h"

VsP_DETECTOR::VsP_DETECTOR(P_DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn *lastCmpInsn) 
:VulnerableSection((DETECTOR*)detector, bb, startInsn, vulnerableInsns, endInsn, isExitSection, prevVulnerableSection, signature, lastCmpInsn){}

rtx_insn* VsP_DETECTOR::prependInsn(bool alternativeImplementation) {
    if(alternativeImplementation) {
        prependParityCheck();
        prependParity(true); // true: add compareLabel
        prependJumpToCheckpoint(true);
        prependExecutionCountCheck();
    } else {
        prependParityCheck();
        prependParity(true); // true: add compareLabel
        prependJumpToCheckpoint();
        prependMarkSecondExecution();
        prependRestore();
        prependParity();
        prependSignatureCheck();
        prependExecutionCountCheck();
    }
    return prependPoint;
}
rtx_insn* VsP_DETECTOR::appendInsn(bool alternativeImplementation) {
    if(alternativeImplementation) {
        appendCheckpoint(false);
        appendOriginalCodeLabel();
        insertJumpToOriginalCode();
        insertParity();
        insertRestore();
        insertMarkSecondExecution();
        appendLastCmp();
    } else {
        appendCheckpoint();
        appendLastCmp();
    }
    return appendPoint;
}
rtx_insn* VsP_DETECTOR::insertStart(bool alternativeImplementation) {
    if (alternativeImplementation) {
        appendCheckpoint(false);
        appendOriginalCodeLabel();
        prependParityInit();
        insertJumpToOriginalCode();
        insertParity();
        insertRestore();
        insertMarkSecondExecution();
    } else {
        prependParityInit();
        appendCheckpoint();
    }
    return appendPoint;
}