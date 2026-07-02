
/**
 * stackleak_test2.c — alloca() placement and the prologue_instrumented path
 *
 * The instrument pass tracks whether alloca() appears in the ENTRY basic
 * block. If it does, it sets prologue_instrumented=true and skips the
 * separate entry-point instrumentation (no double-insert). If alloca() is
 * in a non-entry BB (inside a branch or loop), the pass inserts a call
 * after the alloca AND a separate call at the function entry.
 *
 * Run with: -fplugin-arg-<plugin>-track-min-size=50
 *           -fplugin-arg-<plugin>-verbose
 *
 * Expected "stackleak: instrument" lines:
 *   func_alloca_entry_bb    — alloca in entry BB (prologue_instrumented path)
 *   func_alloca_in_branch   — alloca in non-entry BB (two insertions)
 *   func_alloca_in_loop     — alloca in loop body (non-entry BB)
 *   func_alloca_builtin_align — __builtin_alloca_with_align variant
 *   func_alloca_multiple    — multiple alloca() calls
 *   main
 *
 * NOT expected:
 *   func_no_alloca_small    — no alloca, frame < 50
 */

#include <alloca.h>
#include <string.h>

/* [EXPECT: YES] alloca() is the very first statement — entry BB
 * prologue_instrumented becomes true, entry-point call is NOT duplicated */
void func_alloca_entry_bb(int n) {
    void *p = alloca(n);   /* this is in the entry basic block */
    memset(p, 0, n);
}

/* [EXPECT: YES] alloca() inside an if-branch — non-entry BB
 * instrument pass inserts: (a) call after alloca, (b) call at entry */
void func_alloca_in_branch(int flag, int n) {
    if (flag) {
        void *p = alloca(n);
        memset(p, 0, n);
    }
}

/* [EXPECT: YES] alloca() inside a loop — non-entry BB
 * Each alloca() gets a post-alloca call inserted */
void func_alloca_in_loop(int n) {
    int i;
    for (i = 0; i < n; i++) {
        void *p = alloca(16);
        memset(p, 0, 16);
    }
}

/* [EXPECT: YES] __builtin_alloca_with_align — second is_alloca() branch
 * The plugin checks both BUILT_IN_ALLOCA and BUILT_IN_ALLOCA_WITH_ALIGN */
void func_alloca_builtin_align(int n) {
    /* 128-bit alignment */
    void *p = __builtin_alloca_with_align(n, 128);
    memset(p, 0, n);
}

/* [EXPECT: YES] multiple alloca() calls — each triggers a post-alloca insert */
void func_alloca_multiple(int a, int b) {
    void *p = alloca(a);
    void *q = alloca(b);
    memset(p, 0, a);
    memset(q, 0, b);
}

/* [EXPECT: NO] no alloca, small frame — cleanup removes instrumentation */
int func_no_alloca_small(int x, int y) {
    return x + y;
}

/* [EXPECT: YES] large frame due to locals */
int main(void) {
    char buf[128];
    buf[0] = 0;
    func_alloca_entry_bb(8);
    func_alloca_in_branch(1, 8);
    func_alloca_in_loop(3);
    func_alloca_builtin_align(16);
    func_alloca_multiple(8, 16);
    buf[1] = func_no_alloca_small(1, 2);
    return buf[0];
}