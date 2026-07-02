#include <iostream>
using namespace std;

extern "C" {
    void CFED_Detected(void){
        while(1);
    }
}

// CFED_RSCFC_FULL_STRESS.cpp
// ---------------------------------------------------------------------------
//   PURPOSE:  Exercises all risky code paths in CFED + RSCFC plugins.
//   EFFECTS:  • >100 basic blocks -> bit‑shift overflow in calcVariables()
//             • >50 sequential insns in one BB -> 32‑insn cutoff
//             • Mixed v7‑M / v6‑M targets expose missing 'break' in insertSelBegin()
//             • Many translation units touch same GCC_Plugin_Output directory
//             • Inline‑asm / naked fun disturbs insertSetup() basic block index
// ---------------------------------------------------------------------------

#include <stdint.h>

// ---------------------------------------------------------------------------
// 1) 100+ CFG basic blocks  -> bit‑shift overflow
// ---------------------------------------------------------------------------
int cfg_overflow(int x) {
    int s = 0;
    // Each nested branch adds 2+ basic blocks
    for (int i = 0; i < 60; ++i) {
        if ((x ^ i) & 1)       s += i;
        else if (x < i / 2)    s -= i;
        else if ((x + i) % 3)  s += x * i;
        else if (s & 1)        s >>= 1;
        else                   s ^= i;
        if (s & 2) continue;
    }
    return s;
}

// ---------------------------------------------------------------------------
// 2) One huge linear basic block -> 32‑instruction truncation
// ---------------------------------------------------------------------------
int long_block(int n) {
    int s = n;
#define STEP(k) s += (k);
    STEP(1)  STEP(2)  STEP(3)  STEP(4)  STEP(5)
    STEP(6)  STEP(7)  STEP(8)  STEP(9)  STEP(10)
    STEP(11) STEP(12) STEP(13) STEP(14) STEP(15)
    STEP(16) STEP(17) STEP(18) STEP(19) STEP(20)
    STEP(21) STEP(22) STEP(23) STEP(24) STEP(25)
    STEP(26) STEP(27) STEP(28) STEP(29) STEP(30)
    STEP(31) STEP(32) STEP(33) STEP(34) STEP(35)
    STEP(36) STEP(37) STEP(38) STEP(39) STEP(40)
    STEP(41) STEP(42) STEP(43) STEP(44) STEP(45)
    STEP(46) STEP(47) STEP(48) STEP(49) STEP(50)
#undef STEP
    return s;
}

// ---------------------------------------------------------------------------
// 3) Simple function‑name spam to overflow dirName[512]
//    (call many long‑named functions to make unique folders)
// ---------------------------------------------------------------------------
#define GEN_LONG_NAME(idx) \
    void this_is_a_very_long_function_name_that_should_expand_directory_length_number_##idx(void){}

GEN_LONG_NAME(0)  GEN_LONG_NAME(1)
GEN_LONG_NAME(2)  GEN_LONG_NAME(3)
GEN_LONG_NAME(4)  GEN_LONG_NAME(5)
GEN_LONG_NAME(6)  GEN_LONG_NAME(7)
GEN_LONG_NAME(8)  GEN_LONG_NAME(9)

// ---------------------------------------------------------------------------
// 4) Inline‑assembly function to disturb BASIC_BLOCK_FOR_FN(cfun, 2)
// ---------------------------------------------------------------------------
__attribute__((naked)) void naked_fun(void) {
    asm volatile(
        "nop\n\t"
        "mov r0, r0\n\t"
        "nop\n\t");
}

// ---------------------------------------------------------------------------
// 5) Function using mixed control flow for selective‑mode path duplication
// ---------------------------------------------------------------------------
int selective_bug(int v) {
    int r = v;
    if (v & 1) r += cfg_overflow(v);
    else       r += long_block(v);
    if (r > 1000) naked_fun();
    return r;
}

// ---------------------------------------------------------------------------

int main() {
    for (int i = 0; i < 10; ++i)
        selective_bug(i);
    return 0;
}

