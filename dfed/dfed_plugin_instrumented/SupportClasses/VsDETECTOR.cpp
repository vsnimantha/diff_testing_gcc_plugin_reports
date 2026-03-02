#include "VsDETECTOR.h"

VsDETECTOR::VsDETECTOR(DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn *lastCmpInsn) 
:VulnerableSection(detector, bb, startInsn, vulnerableInsns, endInsn, isExitSection, prevVulnerableSection, signature, lastCmpInsn){}

rtx_insn* VsDETECTOR::prependInsn(bool alternativeImplementation) {
    if (alternativeImplementation) {   
        prependComparison();
        prependJumpToCheckpoint(true);
        prependExecutionCountCheck();
    } else {
        prependComparison();
        prependJumpToCheckpoint();
        prependMarkSecondExecution();
        prependRestore();
        prependBackup();
        prependSignatureCheck();
        prependExecutionCountCheck();
    }
    return prependPoint;
}
rtx_insn* VsDETECTOR::appendInsn(bool alternativeImplementation) {
    if (alternativeImplementation) {
        appendCheckpoint(false);
        appendOriginalCodeLabel();
        insertJumpToOriginalCode();
        insertBackup();
        insertRestore();
        insertMarkSecondExecution();
        appendLastCmp();
    } else {
        appendCheckpoint();
        appendLastCmp();
    }
    return appendPoint;
}
rtx_insn* VsDETECTOR::insertStart(bool alternativeImplementation) {
    if(alternativeImplementation) {
        appendCheckpoint(false);
        appendOriginalCodeLabel();
        insertJumpToOriginalCode();
        insertBackup();
        insertRestore();
        insertMarkSecondExecution();
    } else {
        appendCheckpoint();
    }
    return appendPoint;
}


