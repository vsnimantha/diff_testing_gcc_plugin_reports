/*
 * sancov_test4.c — Maximum BB diversity for sancov_execute
 * ──────────────────────────────────────────────────────────
 * Plain C. No plugin args.
 *
 * Plugin args: none (enable=true path, same as test1)
 *
 * This program maximises diversity of basic block structure so that
 * if sancov_execute() runs, it exercises:
 *   - Self-check: strcmp vs __sanitizer_cov_trace_pc name
 *   - gsi_end_p true  (empty BB after label)  -> continue
 *   - gsi_end_p false (BB with stmts)         -> insert call
 *   - Every function gets its own execute() invocation
 *
 * Uses many short functions to maximise function-level coverage of execute().
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ── trivial 1-BB functions ──────────────────────────────────────
 * Each is a single basic block — tests the non-empty BB path     */
static int square(int x)    { return x * x; }
static int cube(int x)      { return x * x * x; }
static int negate(int x)    { return -x; }
static int is_even(int x)   { return x % 2 == 0; }
static int is_odd(int x)    { return x % 2 != 0; }
static int max2(int a, int b) { return a > b ? a : b; }
static int min2(int a, int b) { return a < b ? a : b; }
static int abs_val(int x)   { return x < 0 ? -x : x; }

/* ── function with many BBs ──────────────────────────────────────*/
static int fizzbuzz(int n)
{
    int count = 0;
    for (int i = 1; i <= n; i++) {
        if (i % 15 == 0)      count += 3;
        else if (i % 3 == 0)  count += 1;
        else if (i % 5 == 0)  count += 2;
    }
    return count;
}

/* ── deeply nested conditions ────────────────────────────────────*/
static int deep_cond(int a, int b, int c, int d)
{
    if (a > 0) {
        if (b > 0) {
            if (c > 0) {
                if (d > 0) return 4;
                else return 3;
            } else {
                return 2;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/* ── multiple return points ──────────────────────────────────────*/
static const char *sign_str(int n)
{
    if (n > 0) return "positive";
    if (n < 0) return "negative";
    return "zero";
}

/* ── switch with many cases ──────────────────────────────────────*/
static int month_days(int m, int leap)
{
    switch (m) {
    case 1: case 3: case 5: case 7:
    case 8: case 10: case 12: return 31;
    case 4: case 6: case 9: case 11: return 30;
    case 2: return leap ? 29 : 28;
    default: return -1;
    }
}

/* ── while with complex exit ──────────────────────────────────────*/
static int collatz_steps(int n)
{
    int steps = 0;
    while (n != 1) {
        if (n % 2 == 0) n /= 2;
        else            n = 3 * n + 1;
        steps++;
        if (steps > 10000) break; /* safety */
    }
    return steps;
}

/* ── goto creates label-only BBs ─────────────────────────────────*/
static int state_machine(int input)
{
    int state = 0;
    int output = 0;


    if (input & 1) { state = 1; goto s1; }
    output += 1;
    goto done;

s1:
    if (input & 2) { state = 2; goto s2; }
    output += 2;
    goto done;

s2:
    output += 4;

done:
    return output + state;
}

/* ── recursive function ───────────────────────────────────────────*/
static int power(int base, int exp)
{
    if (exp == 0) return 1;
    if (exp % 2 == 0) {
        int half = power(base, exp / 2);
        return half * half;
    }
    return base * power(base, exp - 1);
}

/* ── string processing ────────────────────────────────────────────*/
static int count_words(const char *s)
{
    int count = 0, in_word = 0;
    for (; *s; s++) {
        if (*s == ' ' || *s == '\t' || *s == '\n') {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }
    return count;
}

/* ── array with pointer arithmetic ───────────────────────────────*/
static int array_max(int *arr, int n)
{
    if (n <= 0) return 0;
    int m = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > m) m = arr[i];
    return m;
}

int main(void)
{
    printf("%d %d %d\n", square(5), cube(3), negate(7));
    printf("%d %d %d %d\n", is_even(4), is_odd(4), max2(3,7), min2(3,7));
    printf("%d %d\n", abs_val(-5), abs_val(5));

    printf("fizzbuzz(20)=%d\n", fizzbuzz(20));

    printf("deep(%d,%d,%d,%d)=%d\n", 1,1,1,1, deep_cond(1,1,1,1));
    printf("deep(%d,%d,%d,%d)=%d\n", 1,1,1,-1, deep_cond(1,1,1,-1));
    printf("deep(%d,%d,%d,%d)=%d\n", -1,0,0,0, deep_cond(-1,0,0,0));

    printf("sign: %s %s %s\n", sign_str(1), sign_str(-1), sign_str(0));

    for (int m = 1; m <= 12; m++)
        printf("month %d: %d days\n", m, month_days(m, 0));

    printf("collatz(27)=%d steps\n", collatz_steps(27));

    printf("state(0)=%d state(1)=%d state(3)=%d state(7)=%d\n",
           state_machine(0), state_machine(1),
           state_machine(3), state_machine(7));

    printf("2^10=%d\n", power(2, 10));
    printf("3^5=%d\n",  power(3, 5));

    printf("words=%d\n", count_words("hello world foo bar"));
    printf("words=%d\n", count_words("  spaces  "));

    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
    printf("max=%d\n", array_max(arr, 8));

    return 0;
}