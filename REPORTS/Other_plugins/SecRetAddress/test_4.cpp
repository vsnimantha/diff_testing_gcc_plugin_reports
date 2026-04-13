/*
 * instr_test4.cpp — Edge cases: empty functions, deep call chains,
 *                   various array element types
 *
 * Coverage targets:
 *   to_instrument PATH A : functions with zero local variables
 *                          (parameters only, or empty body)
 *   to_instrument PATH B : functions where every local is a scalar/ptr
 *   to_instrument PATH C : arrays of every primitive type
 *   instrument_entry/exit: confirmed across many more function instances
 *   create_clobbers       : single-token vs multi-token paths both hit
 *                           many more times
 */

#include <cstring>

/* ── PATH A: parameter-only functions (no local vars) ─────────── */
int  pass_int   (int x)    { return x; }
long pass_long  (long x)   { return x; }
bool pass_bool  (bool x)   { return x; }
char pass_char  (char x)   { return x; }

/* ── PATH A: empty-body (inline trivial) ─────────────────────── */
struct Empty {
    int val;
    int get() const  { return val; }   /* no locals */
    void set(int v)  { val = v; }      /* no locals */
};

/* ── PATH B: struct local, pointer local, no array ───────────── */
int struct_work(int a, int b)
{
    Empty e;
    e.set(a + b);
    int *p = &e.val;
    int result = *p * 2;
    return result;
}

/* ── PATH C: every primitive array type ─────────────────────────
 * Each function triggers instrument_entry + instrument_exit        */

void use_char_array(char *out)
{
    char buf[32];
    for (int i = 0; i < 32; i++) buf[i] = (char)(i & 0x7f);
    memcpy(out, buf, 32);
}

void use_int_array(int *out)
{
    int buf[32];
    for (int i = 0; i < 32; i++) buf[i] = i * 3;
    memcpy(out, buf, 32 * sizeof(int));
}

void use_long_array(long *out)
{
    long buf[16];
    for (int i = 0; i < 16; i++) buf[i] = (long)i * 1000000L;
    memcpy(out, buf, 16 * sizeof(long));
}

void use_float_array(float *out)
{
    float buf[16];
    for (int i = 0; i < 16; i++) buf[i] = (float)i * 0.1f;
    memcpy(out, buf, 16 * sizeof(float));
}

void use_double_array(double *out)
{
    double buf[8];
    for (int i = 0; i < 8; i++) buf[i] = (double)i * 0.01;
    memcpy(out, buf, 8 * sizeof(double));
}

void use_short_array(short *out)
{
    short buf[32];
    for (int i = 0; i < 32; i++) buf[i] = (short)(i * 2);
    memcpy(out, buf, 32 * sizeof(short));
}

void use_bool_array(bool *out)
{
    bool buf[32];
    for (int i = 0; i < 32; i++) buf[i] = (i % 2 == 0);
    memcpy(out, buf, 32 * sizeof(bool));
}

/* ── PATH C: pointer array ────────────────────────────────────── */
void use_ptr_array(int **out, int *src)
{
    int *ptrs[8];
    for (int i = 0; i < 8; i++) ptrs[i] = src + i;
    memcpy(out, ptrs, 8 * sizeof(int *));
}

/* ── PATH B + C: function with both scalar and array locals ────── */
void mixed_locals(int *out, int n)
{
    int scalar = n * 2;      /* scalar -> does not trigger by itself */
    int arr[16];             /* array -> triggers PATH C */
    for (int i = 0; i < 16; i++) arr[i] = scalar + i;
    memcpy(out, arr, 16 * sizeof(int));
}

int main()
{
    /* PATH A */
    int  r1 = pass_int(1);
    long r2 = pass_long(2L);
    bool r3 = pass_bool(true);
    char r4 = pass_char('x');
    (void)r1; (void)r2; (void)r3; (void)r4;

    Empty e;
    e.set(42);
    int v = e.get();
    (void)v;

    /* PATH B */
    int sw = struct_work(3, 4);
    (void)sw;

    /* PATH C — all types */
    char   ca[32];  use_char_array(ca);
    int    ia[32];  use_int_array(ia);
    long   la[16];  use_long_array(la);
    float  fa[16];  use_float_array(fa);
    double da[8];   use_double_array(da);
    short  sa[32];  use_short_array(sa);
    bool   ba[32];  use_bool_array(ba);

    int  src[8] = {1,2,3,4,5,6,7,8};
    int *pa[8];  use_ptr_array(pa, src);

    int  ma[16]; mixed_locals(ma, 5);

    return 0;
}