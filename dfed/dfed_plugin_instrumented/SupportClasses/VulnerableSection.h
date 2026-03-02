#ifndef VULNERABLE_SECTION_H
#define VULNERABLE_SECTION_H

#include "GeneralDFED.h"
#include "UpdatePoint.h"
#include "AsmGen.h"
#include "InstrType.h"
#include "basic-block.h"
#include <vector>
#include <algorithm>

class DETECTOR;
class P_DETECTOR;

class VulnerableSection
{

public:
    VulnerableSection(DETECTOR *detector, basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn* lastCmpInsn);
    
// -- VIRTUAL ------------------------------------------------------------------------------------------------
    virtual rtx_insn* prependInsn(bool alternativeImplementation) = 0;
    virtual rtx_insn* appendInsn(bool alternativeImplementation)  = 0;
    virtual rtx_insn* insertStart(bool alternativeImplementation) = 0;

// -- OPERATOR OVERLOADS --------------------------------------------------------------------------------------

    bool operator == (const VulnerableSection &s2);
    bool operator != (const VulnerableSection &s2);
    bool operator < (const VulnerableSection &s2);
    bool operator > (const VulnerableSection &s2);
    bool operator <= (const  VulnerableSection &s2);
    bool operator >= (const VulnerableSection &s2);

// -- GETTERS  ------------------------------------------------------------------------------------------------

    /**
     * @return the last compare instruction before this vulnerable section
     */
    rtx_insn *getLastCmpInsn(void);

    /**
     * @return the BB in which the vulnerable section resides
     */
    basic_block &getBB(void);

    /**
     * @return the first instruction of the vulnerable section
     */
    const rtx_insn *const getStartInsn(void);

    /**
     * @return the last instruction of the vulnerable section
     */
    const rtx_insn *const getEndInsn(void);

    /**
     * @return the previous vulnerable instruction that resides in the same bb as this vulnerable section
     */
    VulnerableSection *getPrevVulnerableSection(void);

    /**
     * @return the next vulnerable instruction that resides in the same bb as this vulnerable section
     */
    VulnerableSection *getNextVulnerableSection(void);

    /**
     * @return the signature of this vulnerable section
     */
    unsigned int getSignature(void);

    /**
     * @return the label of the re-execution point (checkpoint) after this vulnerable section
     */
    rtx_insn* getCheckpoint(void);

    /**
     * @return true if this vulnerable section is the first vulnerable section in the basic block
     */
    bool isFirstVulnerableSection(void);

    /**
     * @return true if this vulnerable section is the last vulnerable section in the basic block
     */
    bool isLastVulnerableSection(void);

    /**
     * @return true if the vulnerable section contains a return statement
     */
    bool isExit(void);

    /**
     * @return true if the instruction is part of the VulnerableSection
     */
    bool inVulnerableSection(rtx_insn* insn);

// ------------------------------------------------------------------------------------------------------------

    /**
     * Enable to append last compare instruction before (or in) the vulnerable section after the checkpoint
     * For when a conditional execution depends on a cmp that comes before the vulnerable section
     */

    /**
     * @brief Enable to append last compare instruction before (or in) the vulnerable section after the checkpoint.
     * This is needed for when a conditional execution depends on a cmp that comes before the vulnerable section
     * 
     * @param enable set to true to repeat the last compare instruction
     */
    void repeatLastCmpInsn(bool enable=true);

    /**
     * @brief Set the previous VulnerableSection
     * 
     * @param section The previous vulnerable section
     */
    void setPrevVulnerableSection(VulnerableSection* section);

    /**
     * Set the next VulnerableSection
     * @param section The next vulnerable section
     */
    void setNextVulnerableSection(VulnerableSection* section);

    /**
     * @brief Add source vulnerable sections to this vulnerable section
     * 
     * @param sections The Vulnerable sections to add as a source
     */
    void addSourceVulnerableSections(vector<VulnerableSection *> sections);
    
    /**
     * @brief Remove all source vulnerable sections from this vulnerable section 
     */
    void removeSources(void);

    /**
     * @brief Set the Start Insn object
     * 
     * @param startInsn the new startInsn
     */
    void setStartInsn(rtx_insn *startInsn);

    /**
     * @brief Set a new endInsn
     * 
     * @param endInsn the new endInsn
     */
    void setEndInsn(rtx_insn *endInsn);

    /**
     * @brief Mark the vulnerable section as an exit or not
     * 
     * @param exit true if an exit
     */
    void setExit(bool exit);

    /**
     * @brief Add an instruction to the VulnerableSection
     * 
     * @param insn the instruction to be added
     */
    void addVulnerableInstruction(rtx_insn *insn);
    

protected:
    DETECTOR *detector;
    basic_block bb;
    rtx_insn * startInsn;
    vector<rtx_insn *> vulnerableInsns;
    rtx_insn * endInsn;
    const unsigned int signature;
    bool isExitSection;
    rtx_insn* const lastCmpInsn;
    bool repeatLastCmp;

    VulnerableSection *prevVulnerableSection;
    VulnerableSection *nextVulnerableSection;

    vector<VulnerableSection *> sources;

    rtx_insn *appendPoint;
    rtx_insn *prependPoint;
    rtx_insn *compareLabel;
    rtx_insn *checkpoint;
    rtx_insn *originalCode;

    /**
     * @ Prepend (before the vulnerable section) the instructions to initialize the parity register
     * 
     * @return rtx_insn* 
     */
    rtx_insn *prependParityInit(void);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * check if it is the first or the second execution
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependExecutionCountCheck(void);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * perform the signature check
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependSignatureCheck(void);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * store the first calculation to the first shadow stack
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependBackup(void);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * load the original instructions from the second shadow stack
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependRestore(void);
    
    /**
     * Prepend (before the vulnerable section) the instructions to 
     * mark the beginning of the second execution in the signatureReg
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependMarkSecondExecution(void);
    
    /**
     * Prepend (before the vulnerable section) the instructions to 
     * jump to the checkpoint to start the second execution
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependJumpToCheckpoint(bool alternative = false);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * call the compare subroutine after the second execution
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependComparison(void);

    /**
     * Append (after the vulnerable section) a repeated cmp of the last cmp that occurred
     * This will only be done if enabled using the repeatLastCmpInsn() method
     * 
     * TODO compares that set the compare flags ("ADDS" etc.) should be replaced by a normal CMP to #0
     * 
     * @returns The new appendPoint
     */
    rtx_insn *appendLastCmp(void); // TODO change to a prepend! -- 03/2022 comment: why?
    // FIXME this is a weakpoint of DETECTOR. If the registers of the comparison change in between the repeated compare,
    // the results will not match

    /**
     * Append (after the vulnerable section) the instructions to 
     * add the new checkpoint
     * 
     * @returns The new appendPoint
     */
    rtx_insn *appendCheckpoint(bool setLabel = true);

    /**
     * (For the alternative implementation)
     * Append (after the vulnerable section) a label at the start of the original code.
     * 
     * @return rtx_insn* The new appendPoint
     */
    rtx_insn *appendOriginalCodeLabel(void);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * call the parity subroutine after the second execution
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependParity(bool setCompareLabel = false);

    /**
     * Prepend (before the vulnerable section) the instructions to 
     * check that the parity is 0
     * 
     * @returns The new prependPoint
     */
    rtx_insn *prependParityCheck(void);


    /**
     * Insert (BEFORE the appendInsn) the instructions to 
     * call the parity subroutine after the second execution
     */
    void insertParity(bool setCompareLabel = false);

    /**
     * Insert (BEFORE the appendInsn) the instruction
     * to jump to the original code
     * ! also puts the checkpoint after this jump
     */
    void insertJumpToOriginalCode(void);

    /**
     * Insert (BEFORE the appendInsn) the instruction to 
     * mark the beginning of the second execution in the signatureReg
     */
    void insertMarkSecondExecution(void);

    /**
     * Insert (BEFORE the appendInsn) the instructions to 
     * load the original instructions from the second shadow stack
     */
    void insertRestore(void);

    /**
     * Insert (BEFORE the appendInsn)  the instructions to 
     * store the first calculation to the first shadow stack
     */
    void insertBackup(void);

private:
    /**
     * Insert (BEFORE beforeInsn)  the instructions to 
     * store the first calculation to the first shadow stack
     * 
     * Return the inserted instruction
     */
    rtx_insn *insertBackup(rtx_insn *beforeInsn);

    rtx_insn *insertParity(rtx_insn *beforeInsn);

};
#endif // VULNERABLE_SECTION_H