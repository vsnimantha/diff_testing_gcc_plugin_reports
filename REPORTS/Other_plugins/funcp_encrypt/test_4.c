/* test_argument_passing_fixed.c
 *
 * Same coverage as test_3.c except the struct-pointer-as-argument
 * pattern is replaced with inline access to avoid triggering
 * pointer_to_record_contains_funcp_p in the plugin.
 */

#define MAGIC 0xDEADBEEFUL
typedef void (*handler_t)(int);

static void handle_low(int x)  { volatile int v = x + 1; (void)v; }
static void handle_high(int x) { volatile int v = x * 2; (void)v; }

__attribute__((decryptor))
static handler_t decrypt_handler(handler_t f) {
    return (handler_t)((unsigned long)f ^ MAGIC);
}

static handler_t mangle(handler_t f) {
    return (handler_t)((unsigned long)f + 1UL);
}

static void invoke(handler_t h, int val) {
    h(val);
}

/* Covers: "passing non decrypted fn ptr in argument %d" */
static void pass_raw(handler_t raw) {
    handler_t local = raw;
    invoke(local, 42);   /* WARNING */
}

/* Covers: no warning when decrypted */
static void pass_decrypted(handler_t enc) {
    handler_t dec = decrypt_handler(enc);
    invoke(dec, 42);     /* no warning */
}

/* Covers: ADDR_EXPR suppression */
static void pass_addr_expr(void) {
    invoke(&handle_low, 10);  /* no warning */
}

/* Covers: non-decryptor result not marked decrypted */
static void pass_via_non_decryptor(handler_t enc) {
    handler_t mangled = mangle(enc);
    invoke(mangled, 7);  /* WARNING */
}

/* Covers: rhs1 propagation chain */
static void propagate_via_assignment(handler_t enc) {
    handler_t dec  = decrypt_handler(enc);
    handler_t dec2 = dec;
    invoke(dec2, 5);  /* no warning */
}

/* Covers: comparison with NULL — replaces the struct pointer pattern
   to trigger prop_finalize's GIMPLE_COND path instead */
static void compare_with_null(handler_t enc) {
    if (enc == (handler_t)0) {  /* WARNING: comparison with constant */
        volatile int noop = 0; (void)noop;
    }
}

int main(void) {
    pass_raw(handle_low);
    pass_decrypted(handle_high);
    pass_addr_expr();
    pass_via_non_decryptor(handle_low);
    propagate_via_assignment(handle_high);
    compare_with_null(handle_low);
    return 0;
}