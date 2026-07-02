/*
 * sancov_test3.c — Multiple argv: no-sancov appears after other processing
 * ─────────────────────────────────────────────────────────────────────────
 * Plain C.
 *
 * Plugin args required:
 *   -fplugin-arg-compiled_plugin-no-sancov
 *
 * Covers the same plugin_init enable=false path as test2 but with a
 * program focused on empty basic blocks and label-only BBs, which
 * exercise the gsi_end_p(gsi_after_labels(bb)) -> continue path
 * in sancov_execute() if it ever runs.
 *
 * sancov_execute BB paths:
 *   - BB with only a label and no statements -> gsi_end_p true -> continue
 *   - BB with statements -> gsi_end_p false -> insert call
 *   - Function with single BB
 *   - Function with many BBs
 */

#include <stdio.h>
#include <string.h>

/* Label-heavy code creates BBs that may have no stmts after labels */

static int range_check(int x, int lo, int hi)
{
    /* Each branch target is a BB after a label */
    if (x < lo) goto below;
    if (x > hi) goto above;
    return 0;   /* in-range BB */
below:
    return -1;
above:
    return 1;
}

/* switch with fall-through: compiler may create empty BBs */
static int categorize(int c)
{
    int result = 0;
    switch (c) {
    case 'a': case 'e': case 'i': case 'o': case 'u':
        result = 1; /* vowel */
        break;
    case ' ': case '\t': case '\n':
        result = 2; /* whitespace */
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        result = 3; /* digit */
        break;
    default:
        result = 4;
        break;
    }
    return result;
}

/* Nested conditions with short-circuit — many BBs */
static int valid_range(int a, int b, int c)
{
    return (a > 0 && b > 0 && c > 0) &&
           (a + b > c) &&
           (a + c > b) &&
           (b + c > a);
}

/* Recursion — each call is a fresh function entry for the pass */
static int gcd(int a, int b)
{
    if (b == 0) return a;
    return gcd(b, a % b);
}

/* Function that exits early from multiple points */
static int safe_divide(int a, int b, int *out)
{
    if (!out) return -1;
    if (b == 0) return -2;
    *out = a / b;
    return 0;
}

/* Long linear function: single BB, just many statements */
static int linear_work(int n)
{
    int a = n + 1;
    int b = a * 2;
    int c = b - 3;
    int d = c / 2;
    int e = d + a;
    int f = e * b;
    int g = f - c;
    int h = g + d;
    return h % 97;
}

int main(void)
{
    printf("%d\n", range_check(5, 0, 10));
    printf("%d\n", range_check(-1, 0, 10));
    printf("%d\n", range_check(11, 0, 10));

    const char *str = "Hello World 123";
    for (int i = 0; str[i]; i++)
        printf("%c:%d ", str[i], categorize((unsigned char)str[i]));
    printf("\n");

    printf("valid(3,4,5)=%d\n", valid_range(3, 4, 5));
    printf("valid(1,2,4)=%d\n", valid_range(1, 2, 4));

    printf("gcd(48,18)=%d\n", gcd(48, 18));
    printf("gcd(100,75)=%d\n", gcd(100, 75));

    int result;
    printf("div=%d rc=%d\n", result, safe_divide(10, 3, &result));
    printf("div=%d rc=%d\n", result, safe_divide(10, 0, &result));
    printf("div=%d rc=%d\n", 0, safe_divide(10, 3, NULL));

    printf("linear=%d\n", linear_work(42));

    return 0;
}