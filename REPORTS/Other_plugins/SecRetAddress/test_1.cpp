/*
 * instr_test1.cpp — Baseline: mix of instrumented and skipped functions
 *
 * Coverage targets:
 *   plugin_init PATH B            : version check passes
 *   to_instrument PATH A          : functions with no local vars -> NULL local_decls
 *   to_instrument PATH B          : functions with scalars but no arrays -> return false
 *   to_instrument PATH C          : functions with local arrays -> return true
 *   instrument_functions PATH A   : early return when not instrumented
 *   instrument_functions PATH B   : full instrumentation when array found
 *   instrument_entry              : all 4 create_clobbers + asm insertions
 *   instrument_exit               : clobbers + asm insertion
 *   create_clobbers / loop body   : multi-token strings like "r8,r9,ecx,edx,esi,edi,rax"
 *   create_clobbers / single token: "memory", "rcx,memory"
 *   build_const_char_string       : called for every ASM constraint
 *   create_pointer                : called once per instrumented function
 *
 * CRITICAL: every instrumented function must have exactly ONE return point
 * because instrument_exit calls single_pred(EXIT_BLOCK) which crashes
 * if there are multiple predecessors (multiple return statements).
 */

#include <cstring>

/* ── PATH A: no local vars -> local_decls == NULL -> skip ─────── */
int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

/* ── PATH B: local scalars only, no arrays -> skip ───────────── */
int scalar_work(int n)
{
    int a = n * 2;
    int b = a + 3;
    int c = b * b;
    int result = c - a;
    return result;
}

int pointer_work(int n)
{
    int x = n;
    int *p = &x;
    int *q = p;
    int result = *q + 1;
    return result;
}

/* ── PATH C: local char array -> instrument entry+exit ───────── */
/* Single return point - required for single_pred() */
void fill_buffer(char *out, int n)
{
    char buf[128];
    for (int i = 0; i < n && i < 127; i++)
        buf[i] = (char)('a' + (i % 26));
    buf[n < 128 ? n : 127] = '\0';
    memcpy(out, buf, 128);
}

/* ── PATH C: local int array -> instrument ───────────────────── */
void init_array(int *out, int n)
{
    int arr[64];
    for (int i = 0; i < 64; i++)
        arr[i] = i * n;
    memcpy(out, arr, 64 * sizeof(int));
}

/* ── PATH C: multiple local arrays -> instrument once ────────── */
void multi_array(int *out)
{
    int   a[16];
    float b[16];
    for (int i = 0; i < 16; i++) {
        a[i] = i;
        b[i] = (float)i * 1.5f;
        out[i] = a[i] + (int)b[i];
    }
}

int main()
{
    /* Trigger PATH A functions */
    int r1 = add(3, 4);
    int r2 = subtract(10, 3);
    int r3 = multiply(6, 7);
    (void)r1; (void)r2; (void)r3;

    /* Trigger PATH B functions */
    int r4 = scalar_work(5);
    int r5 = pointer_work(10);
    (void)r4; (void)r5;

    /* Trigger PATH C functions */
    char out[128];
    fill_buffer(out, 10);

    int arr[64];
    init_array(arr, 3);

    int mixed[16];
    multi_array(mixed);

    return 0;
}