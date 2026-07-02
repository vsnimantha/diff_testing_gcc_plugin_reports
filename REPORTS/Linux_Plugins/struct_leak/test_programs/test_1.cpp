
/**
 * stackleak_test1.c — Basic frame-size and alloca instrumentation paths
 *
 * Tests the two fundamental reasons the cleanup pass KEEPS instrumentation:
 *   1. Stack frame >= track-min-size
 *   2. Function calls alloca()
 *
 * Run with: -fplugin-arg-<plugin>-track-min-size=100
 *           -fplugin-arg-<plugin>-verbose
 *
 * Expected stdout lines containing "stackleak: instrument":
 *   func_large_frame      — frame >= 100 bytes
 *   func_uses_alloca      — calls alloca
 *   func_alloca_and_large — both reasons
 *   main                  — large frame (local arrays)
 *
 * NOT expected (cleanup removes these):
 *   func_small_frame      — frame < 100 bytes, no alloca
 *   func_empty            — no locals at all
 */

#include <alloca.h>
#include <string.h>

/* [EXPECT: NO] frame below threshold, no alloca */
int func_small_frame(int x) {
    int a = x + 1;
    int b = a * 2;
    return b;
}

/* [EXPECT: YES] frame >= threshold (100 bytes) */
int func_large_frame(int x) {
    char buf[128];   /* 128 bytes >= threshold of 100 */
    buf[0] = x;
    buf[127] = x + 1;
    return buf[0] + buf[127];
}

/* [EXPECT: YES] calls alloca — cleanup pass preserves regardless of frame size */
void func_uses_alloca(int n) {
    void *p = alloca(n);
    memset(p, 0, n);
}

/* [EXPECT: YES] both alloca AND large frame */
int func_alloca_and_large(int n) {
    char buf[200];
    void *p = alloca(n);
    buf[0] = n;
    memset(p, 0, n);
    return buf[0];
}

/* [EXPECT: NO] completely empty body — no frame, no alloca */
void func_empty(void) {
}

/* [EXPECT: NO] only a scalar local — frame < threshold */
int func_scalar_only(int x) {
    int result = x * x;
    return result;
}

/* [EXPECT: YES] main has locals pushing frame over threshold */
int main(void) {
    char workspace[256];
    workspace[0] = func_small_frame(1);
    workspace[1] = func_large_frame(2);
    func_uses_alloca(32);
    workspace[2] = func_alloca_and_large(16);
    func_empty();
    workspace[3] = func_scalar_only(4);
    return workspace[0];
}