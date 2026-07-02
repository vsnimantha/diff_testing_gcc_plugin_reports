// test_start_unit.c
// Compile: gcc -O1 -fstack-protector-all
//          -fplugin=./arm_pertask_ssp.so
//          -fplugin-arg-arm_pertask_ssp-offset=16
//          test_start_unit.c -o test_start_unit
//
// The PLUGIN_START_UNIT callback fires once before RTL for this TU,
// overriding targetm.have_stack_protect_combined_set/test to return false.
// Functions without buffers produce no canary SET insn — the RTL loop
// hits INSN_P() failures and NEXT_INSN() continues.

#include <stdio.h>
#include <string.h>

/* No local buffer — no canary — exercises the non-matching INSN path */
int add(int a, int b) {
    return a + b;
}

/* No local buffer */
int multiply(int a, int b) {
    return a * b;
}

/* Has a buffer — generates a canary SET insn — exercises the match path */
void format_result(int val) {
    char out[128];
    snprintf(out, sizeof(out), "result = %d", val);
    puts(out);
}

/* Variadic — forces a more complex frame, still generates canary */
void log_values(int count, ...) {
    char msg[256];
    __builtin_va_list ap;
    __builtin_va_start(ap, count);
    int i, total = 0;
    for (i = 0; i < count; i++)
        total += __builtin_va_arg(ap, int);
    __builtin_va_end(ap);
    snprintf(msg, sizeof(msg), "sum of %d values: %d", count, total);
    puts(msg);
}

int main(void) {
    format_result(add(3, 4));
    log_values(3, 10, 20, 30);
    return multiply(2, 3);
}