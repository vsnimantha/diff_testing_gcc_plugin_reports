/* test_linear.c
 *
 * Compile (ARM cross-compiler, ARMv7-M, CFCSS as example technique):
 *
 *   arm-none-eabi-gcc-7.3 -O1 -mcpu=cortex-m4 -mthumb
 *     -ffixed-r6 -ffixed-r10 -ffixed-r11 -fomit-frame-pointer
 *     -fno-jump-tables
 *     -fplugin=./CFED_plugin64.so
 *     -fplugin-arg-CFED_plugin64-function=all
 *     -fplugin-arg-CFED_plugin64-techniqueType=SigMon
 *     -fplugin-arg-CFED_plugin64-techniqueSpecific=CFCSS
 *     -fplugin-arg-CFED_plugin64-selectiveLevel=0
 *     test_linear.c -o test_linear.elf
 *
 * What it covers:
 *   - plugin_init: all four arguments parsed, function="all",
 *     techniqueType="SigMon", techniqueSpecific="CFCSS", selectiveLevel=0
 *   - RTL pass: purely linear CFG — one entry block, sequential basic
 *     blocks with no branches, one exit block.
 *   - Exercises the "insert signature update at every block" path
 *     (selectiveLevel=0) with a trivially provable control flow.
 */

#include <stdint.h>

/* Simple linear computation — the compiler will produce a small,
   branch-free sequence of basic blocks for each function. */

extern "C" {
    void CFED_Detected(void){
        while(1);
    }
}

uint32_t checksum(const uint8_t *data, uint32_t len) {
    uint32_t sum = 0;
    uint32_t i;
    for (i = 0; i < len; i++) {
        sum += data[i];
        sum ^= (sum << 3);
        sum += (sum >> 5);
    }
    return sum;
}

uint32_t scale(uint32_t val, uint32_t factor) {
    uint32_t result = val * factor;
    result >>= 2;
    result += 1;
    return result;
}

int main(void) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x08};
    uint32_t c = checksum(data, 5);
    uint32_t s = scale(c, 3);
    return (int)(s & 0xFF);
}