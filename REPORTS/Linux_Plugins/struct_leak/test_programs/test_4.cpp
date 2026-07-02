
/**
 * stackleak_test3.c — Gate function: section attributes and skip conditions
 *
 * stackleak_gate() returns false (plugin skips the function entirely) when:
 *   1. track_frame_size < 0  (no track-min-size arg)
 *   2. Function has a section attribute in the kernel-reserved list:
 *      .init.text, .devinit.text, .cpuinit.text, .meminit.text,
 *      .noinstr.text, .entry.text, .head.text
 *
 * The instrument pass also skips (early return, NOT gate):
 *   3. is_leaf AND !TREE_PUBLIC AND DECL_DECLARED_INLINE
 *      -> static inline leaf functions
 *   4. is_leaf AND name starts with "_paravirt_"
 *      -> paravirt virtual functions
 *
 * Run with: -fplugin-arg-<plugin>-track-min-size=0
 *           -fplugin-arg-<plugin>-verbose
 *
 * Expected "stackleak: instrument":
 *   func_normal_small      — no special section, normal function
 *   func_normal_non_leaf   — calls another function so is_leaf=false
 *   func_not_inline_static — static but NOT inline, so not skipped
 *   main
 *
 * NOT expected:
 *   func_init_section      — .init.text gate returns false
 *   func_noinstr_section   — .noinstr.text gate returns false
 *   func_entry_section     — .entry.text gate returns false
 *   func_static_inline_leaf — static inline leaf — instrument skips
 *   _paravirt_leaf_func    — _paravirt_ prefix leaf — instrument skips
 */

/* Gate skip: .init.text section */
__attribute__((section(".init.text")))
int func_init_section(int x) {
    /* [EXPECT: NO] gate returns false for .init.text */
    char buf[64];
    buf[0] = x;
    return buf[0];
}

/* Gate skip: .noinstr.text section */
__attribute__((section(".noinstr.text")))
int func_noinstr_section(int x) {
    /* [EXPECT: NO] gate returns false for .noinstr.text */
    char buf[64];
    buf[0] = x;
    return buf[0];
}

/* Gate skip: .entry.text section */
__attribute__((section(".entry.text")))
void func_entry_section(void) {
    /* [EXPECT: NO] gate returns false for .entry.text */
}

/* Instrument-pass skip: static inline leaf (no calls inside) */
static inline int func_static_inline_leaf(int x) {
    /* [EXPECT: NO] is_leaf=true, !TREE_PUBLIC, DECL_DECLARED_INLINE */
    return x * 2;
}

/* Instrument-pass skip: _paravirt_ prefix + leaf */
/* NOTE: in kernel context this would be noinline; here we rely on
 * the compiler not inlining the call inside it (it calls nothing) */
static int _paravirt_leaf_func(int x) {
    /* [EXPECT: NO] name starts with _paravirt_, is_leaf=true */
    return x + 1;
}

/* [EXPECT: YES] normal function, no special section */
int func_normal_small(int x) {
    return x + _paravirt_leaf_func(x);
}

/* [EXPECT: YES] static but NOT inline — DECL_DECLARED_INLINE is false */
static int func_not_inline_static(int x) {
    char buf[16];
    buf[0] = x;
    return buf[0];
}

/* [EXPECT: YES] non-leaf (calls func_not_inline_static) —
 * is_leaf=false so the static-inline-leaf skip does not apply */
int func_normal_non_leaf(int x) {
    return func_not_inline_static(x) + func_normal_small(x);
}

/* [EXPECT: YES] main — large frame, non-leaf */
int main(void) {
    char buf[128];
    buf[0] = func_init_section(1);
    buf[1] = func_noinstr_section(2);
    func_entry_section();
    buf[2] = func_static_inline_leaf(3);
    buf[3] = func_normal_small(4);
    buf[4] = func_not_inline_static(5);
    buf[5] = func_normal_non_leaf(6);
    return buf[0];
}