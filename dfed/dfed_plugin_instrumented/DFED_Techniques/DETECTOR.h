/*
*
*   Aug 31, 2021
*
*/
#ifndef DETECTOR_H
#define DETECTOR_H

#include "GeneralDFED.h"
#include "UpdatePoint.h"
#include "AsmGen.h"
#include "InstrType.h"
#include "basic-block.h"
#include <vector>
#include <algorithm>
#include "VulnerableSection.h"
#include "VsDETECTOR.h"

class DETECTOR : public GeneralDFED
{
public:
    // TODO the  regmap is just for the GeneralDFED inheritance (and the usedRegMap initialisation)
    DETECTOR(ARM_ISA* isa, map<unsigned char, unsigned char> regmap, unsigned char signatureReg, unsigned char ssPtr1, unsigned char ssPtr2, vector<unsigned char> protectedRegs);
    ~DETECTOR() {}

    /**
     * Entry point called by the DFEDcreator class
     */
    virtual void implementTechnique(void) override;

    /**
     * Enable or disable the alternative implementation for the current function
     * 
     * @param enable True if it should be enabled, otherwise false.
     */
    void setAlternativeImplementation(bool enable);

// -- GETTERS  ------------------------------------------------------------------------------------------------
    unsigned char getSignatureReg(void);
    unsigned char getSsPtr1(void);
    unsigned char getSsPtr2(void);
    ARM_ISA &getIsa(void);
    vector<unsigned char> &getUsedRegs(void);
    vector<unsigned char> &getProtectedRegs(void);
    bool getSpProtected(void);
    char *getCompareSubroutine(void);
    rtx_insn *getErrorLabel(void);
    VulnerableSection *getVulnerableSection(rtx_insn *insn);
// ------------------------------------------------------------------------------------------------------------

protected:
    unsigned char signatureReg;
    unsigned char ssPtr1;
    unsigned char ssPtr2;
    vector<unsigned char> protectedRegs;
    bool spProtected;
    unsigned int CMPlimit; // for random process
    char* compareSubroutine;
    bool alternativeImplementation;
    vector<VulnerableSection *> vulnerableSections;
    vector<unsigned int> signatures;
    VulnerableSection* startPoint;
    vector<unsigned char> usedRegs;
    rtx_insn* errorLabel;
    vector<rtx_insn*> protectedStrSp;   // str [sp] instructions that are protected

    virtual VulnerableSection *createVulnerableSection(basic_block &bb, rtx_insn *const startInsn, vector<rtx_insn *> vulnerableInsns, rtx_insn *const endInsn, const bool isExitSection, VulnerableSection *prevVulnerableSection, unsigned int signature, rtx_insn* lastCmpInsn);

// -- implementTechnique ---------------------------------------------------------------------------------------------------

    /**
     * Find all the registers that are used in the function & mark the m in `usedRegMap`
     */
    void findUsedRegisters(void);

    /**
     * Insert the instructions at the begining of the function
     */
    void insertSetup(void) override;

    /**
     * Find all the vulnerable sections in the program, save them in the `vulnerableSections` vector
     * 
     * @param strSp set true to search specifically for vulnerable stores to the sp. Defaults to false
     */
    void findVulnerableSections(bool strSp = false);

    /**
     * Clear the sources stored in the VulnerableBlocks
     */
    void cleanReExecutionPoints(void);

    /**
     * Find all re-execution points in the program (i.e. sources of all vulnerable blocks)
     * Update all vulnerable blocks with the sources
     */
    void findReExecutionPoints(void);

    /**
     * Insert the instructions that make up the DETECTOR technique
     */
    void insertInstructions(void);

// ------------------------------------------------------------------------------------------------------------------

    /**
     * Mark all the registers in an instruction as used 
     * (recursive method)
     * @param instruction The expression of the instruction
     */
    void markUsedRegisters(rtx instruction);

    /**
     * Generates a unique random signature
     * @returns the signature
     */
    unsigned int generateSignature(void);

    /**
     * @param signature The signature of which the uniqueness should be checked
     * @returns true if the signature is unique
     */
    bool isUniqueSignature(unsigned int signature);

    /**
     * @param instruction the instruction of which should be checked if it is vulnerable
     * @param strSp set to true if the check should be specifically for stores to sp
     * @returns true if the instruction is considered vulnerable by DETECTOR
     */
    bool isVulnerableInsn(rtx_insn *instruction, basic_block bb, bool strSp);

    /**
     * Find the previous "ldr <reg> [sp, #<offset>]" in the control flow of the program in the previous re-execution block.
     * Recursive function
     * @param reg The register to which the load has to write
     * @param offset The offset from the stack pointer
     * @param bb The current bb which is being searched through
     * @param startInsn (optional) The instruction from which the search should start
     * @return The found "ldr <reg> [sp, #<offset>]" instruction or nullptr if none is found
     */
    rtx_insn *findPrevLdrSpInReExecution(unsigned char reg, int offset, basic_block bb, rtx_insn *startInsn = nullptr);

    /**
     * Check if an instruction is the last instruction in a basic block
     * @param instruction the instruction of which should be checked if it is vulnerable
     * @param bb the basic block in which the instruction resides
     * @returns true is the instruction in the basic block
     */
    bool isLastInsn(rtx_insn *instruction, basic_block bb);

    /**
     * Find all sources of a basic block: a BB with a vulnerable section or the enty point BB
     * (recursive function)
     * @param bb The basic block of which the sources should be found
     * @param vulnerableSections A list of all the vulnerable sections
     * @param traversed A list of indexes of all BB's that have already been traversed
     * @param foundSources All the sources (Vulnerable Sections)
     */
    vector<VulnerableSection *> findSources(basic_block const &bb, vector<VulnerableSection *> const &vulnerableSections,     vector<int> &traversed, vector<VulnerableSection* > &foundSources);

    /**
     * Add instructions to the startPoint to comply with the alternative implementation
     */
    void insertStart(void);
};

#endif