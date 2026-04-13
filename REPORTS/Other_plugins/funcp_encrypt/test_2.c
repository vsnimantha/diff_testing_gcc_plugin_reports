/* test_phi_comparisons.c
 *
 * Compile:
 *   gcc -fplugin=./funcp_plugin.so test_phi_comparisons.c -O1 -o test2
 *
 * What it covers:
 *   prop_visit_phi_node — all incoming edges decrypted → PHI decrypted
 *   prop_visit_phi_node — one edge not decrypted → NOT_INTERESTING
 *   prop_visit_stmt    — stmt_ends_bb_p → SSA_PROP_VARYING (successor blocks)
 *   prop_finalize      — "comparison between encrypted fn pointer and constant"
 *   prop_finalize      — "comparison between different encrypted fn pointers"
 */

#define MAGIC 0xDEADBEEFUL

typedef void (*fn_t)(void);

static void f1(void) { volatile int x = 10; (void)x; }
static void f2(void) { volatile int x = 20; (void)x; }

/* Covers: comparison-with-NULL warning
   GIMPLE_COND: encrypted_fn == NULL */
static void compare_with_null(fn_t enc) {
    if (enc == (fn_t)0) {   /* WARNING: comparison between encrypted fn ptr and constant */
        volatile int noop = 0; (void)noop;
    }
}

/* Covers: mixed-encryption comparison warning
   dec is decrypted, also_enc is not */
static void compare_mixed(fn_t enc, fn_t also_enc) {
    fn_t dec = (fn_t)((unsigned long)enc ^ MAGIC);
    if (dec == also_enc) {  /* WARNING: comparison between different encrypted fn ptrs */
        volatile int noop = 0; (void)noop;
    }
}

/* Covers: prop_visit_phi_node — both branches decrypt before merge
   PHI result is decrypted on all incoming edges → marked decrypted */
static void phi_all_decrypted(fn_t enc, int flag) {
    fn_t result;
    if (flag) {
        result = (fn_t)((unsigned long)enc ^ MAGIC);
    } else {
        result = (fn_t)((unsigned long)enc ^ MAGIC);
    }
    result();   /* no warning — PHI result is decrypted */
}

/* Covers: prop_visit_phi_node — one edge raw → NOT_INTERESTING
   PHI result is not marked decrypted */
static void phi_one_raw(fn_t enc, int flag) {
    fn_t result;
    if (flag) {
        result = (fn_t)((unsigned long)enc ^ MAGIC); /* decrypted */
    } else {
        result = enc;                                 /* not decrypted */
    }
    result();   /* WARNING: possible use of non decrypted function pointer */
}

int main(void) {
    compare_with_null(f1);
    compare_mixed(f1, f2);
    phi_all_decrypted(f1, 1);
    phi_one_raw(f2, 0);
    return 0;
}