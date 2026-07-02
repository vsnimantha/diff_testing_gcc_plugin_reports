// test_args.c
// Run 4 times to hit all plugin_init branches:
//
// (a) No args — baseline path, registers pass, returns 0
//     gcc -fplugin=./arm_pertask_ssp.so test_args.c -o /dev/null
//
// (b) offset arg — sets canary_offset, continues loop
//     gcc -fplugin=./arm_pertask_ssp.so
//         -fplugin-arg-arm_pertask_ssp-offset=32
//         test_args.c -o /dev/null
//
// (c) disable arg — early return 0, skips pass registration
//     gcc -fplugin=./arm_pertask_ssp.so
//         -fplugin-arg-arm_pertask_ssp-disable
//         test_args.c -o /dev/null
//
// (d) unknown key — hits error() + return 1
//     gcc -fplugin=./arm_pertask_ssp.so
//         -fplugin-arg-arm_pertask_ssp-badkey=val
//         test_args.c -o /dev/null

#include <stdio.h>

int main(void) {
    /* Minimal translation unit — we are testing plugin_init, not the RTL pass */
    printf("ok\n");
    return 0;
}