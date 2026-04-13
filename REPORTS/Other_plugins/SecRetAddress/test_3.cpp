/*
 * instr_test3.cpp — Nested functions, templates, many array types
 *
 * Coverage targets:
 *   to_instrument PATH C : many more array-containing functions
 *   to_instrument PATH A : inline/trivial functions (no locals)
 *   create_clobbers loop : exercised many more times
 *   instrument_entry/exit: many function instances from templates
 *
 * All instrumented functions have exactly ONE return point.
 */

#include <cstring>

/* ── Template functions: each instantiation is a separate function ──
 * Each instantiation with a local array -> PATH C per type           */
template<typename T, int N>
void array_fill(T *out, T val)
{
    T tmp[N];
    for (int i = 0; i < N; i++) tmp[i] = val;
    memcpy(out, tmp, N * sizeof(T));
}

template<typename T, int N>
T array_sum(T *in)
{
    T tmp[N];
    memcpy(tmp, in, N * sizeof(T));
    T s = T(0);
    for (int i = 0; i < N; i++) s += tmp[i];
    return s;
}

/* ── Nested local arrays ─────────────────────────────────────── */
void nested_arrays(int *out)
{
    int outer[16];
    for (int i = 0; i < 16; i++) {
        int inner[4];
        for (int j = 0; j < 4; j++) inner[j] = i * j;
        outer[i] = inner[0] + inner[1] + inner[2] + inner[3];
    }
    memcpy(out, outer, 16 * sizeof(int));
}

/* ── 2D array ────────────────────────────────────────────────── */
void matrix_mul(int *out)
{
    int a[4][4];
    int b[4][4];
    int c[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            a[i][j] = i + j;
            b[i][j] = i - j;
        }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            c[i][j] = 0;
            for (int k = 0; k < 4; k++)
                c[i][j] += a[i][k] * b[k][j];
        }
    memcpy(out, c, 16 * sizeof(int));
}

/* ── Array of structs (ARRAY_TYPE of RECORD_TYPE) ────────────── */
struct Point { int x; int y; };

void fill_points(Point *out, int n)
{
    Point pts[32];
    for (int i = 0; i < 32; i++) { pts[i].x = i; pts[i].y = i * 2; }
    memcpy(out, pts, n * sizeof(Point));
}

/* ── PATH B: only scalars ────────────────────────────────────── */
int dot_product(int ax, int ay, int bx, int by)
{
    int r = ax * bx + ay * by;
    return r;
}

/* ── PATH A: no locals ───────────────────────────────────────── */
int identity(int x)       { return x; }
bool is_positive(int x)   { return x > 0; }

int main()
{
    /* Template instantiations */
    int   ibuf[16];  array_fill<int,   16>(ibuf, 7);
    float fbuf[16];  array_fill<float, 16>(fbuf, 3.14f);
    char  cbuf[16];  array_fill<char,  16>(cbuf, 'A');

    int   isum = array_sum<int,   16>(ibuf);
    float fsum = array_sum<float, 16>(fbuf);
    (void)isum; (void)fsum;

    int out[16];
    nested_arrays(out);

    int mat[16];
    matrix_mul(mat);

    Point pts[32];
    fill_points(pts, 10);

    int dp = dot_product(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
    (void)dp;

    int id = identity(42);
    bool pos = is_positive(-1);
    (void)id; (void)pos;

    return 0;
}