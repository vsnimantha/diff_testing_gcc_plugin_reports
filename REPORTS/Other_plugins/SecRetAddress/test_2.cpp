/*
 * instr_test2.cpp — C++ classes with array members
 *
 * Coverage targets (same plugin paths, more function instances):
 *   to_instrument PATH C : constructor/destructor/methods with local arrays
 *   to_instrument PATH B : methods with only scalar locals
 *   instrument_entry/exit: multiple class methods instrumented
 *   create_clobbers       : called many times across many functions
 *
 * All instrumented functions have exactly ONE return point.
 */

#include <cstring>

/* ── Class with methods that have local arrays ───────────────── */
class Buffer {
    int data[32];
    int size;
public:
    Buffer()
    {
        /* local array in constructor -> PATH C */
        int tmp[32];
        for (int i = 0; i < 32; i++) tmp[i] = 0;
        memcpy(data, tmp, sizeof(tmp));
        size = 32;
    }

    void fill(int val)
    {
        /* local array -> PATH C, single return */
        int tmp[32];
        for (int i = 0; i < 32; i++) tmp[i] = val;
        memcpy(data, tmp, sizeof(tmp));
    }

    int sum() const
    {
        /* no local array -> PATH B */
        int s = 0;
        for (int i = 0; i < size; i++) s += data[i];
        return s;
    }

    int get(int i) const { return data[i]; } /* PATH A: no locals */
    int getSize() const  { return size; }    /* PATH A: no locals */
};

/* ── Class with char array member ────────────────────────────── */
class StringBuf {
    char buf[64];
    int  len;
public:
    StringBuf()
    {
        char tmp[64];
        memset(tmp, 0, 64);
        memcpy(buf, tmp, 64);
        len = 0;
    }

    void set(const char *s, int n)
    {
        /* local char array -> PATH C */
        char tmp[64];
        int  copy = n < 63 ? n : 63;
        memcpy(tmp, s, copy);
        tmp[copy] = '\0';
        memcpy(buf, tmp, 64);
        len = copy;
    }

    int length() const { return len; }  /* PATH A */
};

/* ── Free functions with various array sizes ─────────────────── */

void process_small(int *out)
{
    int arr[8];
    for (int i = 0; i < 8; i++) arr[i] = i * i;
    memcpy(out, arr, 8 * sizeof(int));
}

void process_large(double *out)
{
    double arr[256];
    for (int i = 0; i < 256; i++) arr[i] = (double)i / 3.14;
    memcpy(out, arr, 256 * sizeof(double));
}

/* Scalar only -> PATH B */
double compute(double x, double y)
{
    double a = x * x;
    double b = y * y;
    double c = a + b;
    return c;
}

int main()
{
    Buffer b;
    b.fill(7);
    int s = b.sum();
    int v = b.get(0);
    int sz = b.getSize();
    (void)s; (void)v; (void)sz;

    StringBuf sb;
    sb.set("hello", 5);
    int l = sb.length();
    (void)l;

    int small[8];
    process_small(small);

    double large[256];
    process_large(large);

    double r = compute(3.0, 4.0);
    (void)r;

    return 0;
}