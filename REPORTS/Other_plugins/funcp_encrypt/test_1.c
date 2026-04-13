/* test_basic_propagation.c
 *
 * Compile:
 *   gcc -fplugin=./funcp_plugin.so test_basic_propagation.c -O1 -o test1
 *
 * What it covers:
 *   prop_visit_stmt — BIT_XOR_EXPR marks result decrypted
 *   prop_visit_stmt — BIT_IOR_EXPR marks result decrypted
 *   prop_visit_stmt — BIT_AND_EXPR marks result decrypted
 *   prop_visit_stmt — call_is_decryptor_p true path
 *   prop_finalize   — "possible use of non decrypted function pointer"
 *                     (local SSA copy of PARM_DECL, not unmodified)
 *   unmodified_parm_decl_p — suppresses warning for raw PARM_DECL cb()
 */

#define MAGIC 0xDEADBEEFUL

typedef void (*callback_t)(void);

/* __attribute__((decryptor)) — call_is_decryptor_p returns true */
__attribute__((decryptor))
static callback_t decrypt_cb(callback_t f) {
    return (callback_t)((unsigned long)f ^ MAGIC);
}

static void action_a(void) { volatile int x = 1; (void)x; }
static void action_b(void) { volatile int x = 2; (void)x; }

/* Covers: BIT_XOR_EXPR — result is marked decrypted, no warning */
static void call_via_xor(callback_t enc) {
    callback_t tmp = (callback_t)((unsigned long)enc ^ MAGIC);
    tmp();   /* no warning */
}

/* Covers: BIT_IOR_EXPR */
static void call_via_ior(callback_t enc) {
    callback_t tmp = (callback_t)((unsigned long)enc | 0x1UL);
    tmp();   /* no warning */
}

/* Covers: BIT_AND_EXPR */
static void call_via_and(callback_t enc) {
    callback_t tmp = (callback_t)((unsigned long)enc & ~0x1UL);
    tmp();   /* no warning */
}

/* Covers: call_is_decryptor_p true — result of decryptor call is decrypted */
static void call_via_decryptor(callback_t enc) {
    callback_t tmp = decrypt_cb(enc);
    tmp();   /* no warning */
}

/* Covers: unmodified_parm_decl_p — raw PARM_DECL, no warning */
static void call_raw_parm(callback_t cb) {
    cb();    /* no warning — cb is unmodified PARM_DECL */
}

/* Covers: "possible use of non decrypted function pointer"
   local is an SSA copy, not an unmodified PARM_DECL */
static void call_raw_copy(callback_t cb) {
    callback_t local = cb;
    local(); /* WARNING */
}

int main(void) {
    call_via_xor(action_a);
    call_via_ior(action_a);
    call_via_and(action_b);
    call_via_decryptor(action_b);
    call_raw_parm(action_a);
    call_raw_copy(action_b);
    return 0;
}