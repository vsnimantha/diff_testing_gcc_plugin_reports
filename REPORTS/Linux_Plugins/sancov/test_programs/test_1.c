/*
 * sancov_test1.c — Baseline: no plugin args
 * ──────────────────────────────────────────
 * Plain C. No plugin args.
 *
 * plugin_init paths covered:
 *   - plugin_default_version_check pass
 *   - argv loop body NOT entered (argc=0)
 *   - register_callback PLUGIN_INFO
 *   - enable=true -> fall through (gcc>=6: nothing else registered)
 *
 * sancov_execute paths (if registered):
 *   - many BBs with statements -> gsi_end_p false -> insert gcall
 *   - empty BB after label -> gsi_end_p true -> continue
 *   - self-function strcmp check
 */

#include <stdio.h>
#include <string.h>

/* Many functions = many sancov_execute() invocations */

static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }

/* Chains of if/else -> many BBs per function */
static int classify(int n)
{
    if (n < 0)
        return -1;
    else if (n == 0)
        return 0;
    else if (n < 10)
        return 1;
    else if (n < 100)
        return 2;
    else
        return 3;
}

/* Loop -> multiple BBs: loop header, body, exit */
static int sum_to(int n)
{
    int s = 0;
    for (int i = 1; i <= n; i++)
        s += i;
    return s;
}

/* Switch -> one BB per case */
static const char *day_name(int d)
{
    switch (d) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
    default: return "?";
    }
}

/* goto -> explicit label creates a BB boundary */
static int goto_example(int n)
{
    int result = 0;
    if (n <= 0) goto done;
    result = n * 2;
    if (result > 100) goto cap;
    goto done;
cap:
    result = 100;
done:
    return result;
}

/* Nested loops -> deeply nested BB structure */
static int matrix_trace(int n)
{
    int trace = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                trace += i * j + 1;
        }
    }
    return trace;
}

/* while + break/continue -> extra BB edges */
static int first_negative(int *arr, int len)
{
    int i = 0;
    while (i < len) {
        if (arr[i] < 0) break;
        i++;
    }
    return (i < len) ? arr[i] : 0;
}

/* do-while -> at least one execution BB */
static int digit_count(int n)
{
    int count = 0;
    if (n < 0) n = -n;
    do {
        count++;
        n /= 10;
    } while (n > 0);
    return count;
}

int main(void)
{
    printf("%d\n", add(3, 4));
    printf("%d\n", sub(10, 3));
    printf("%d\n", mul(6, 7));
    printf("%d\n", classify(-5));
    printf("%d\n", classify(0));
    printf("%d\n", classify(5));
    printf("%d\n", classify(50));
    printf("%d\n", classify(500));
    printf("%d\n", sum_to(10));
    printf("%s\n", day_name(3));
    printf("%s\n", day_name(9));
    printf("%d\n", goto_example(0));
    printf("%d\n", goto_example(40));
    printf("%d\n", goto_example(200));
    printf("%d\n", matrix_trace(4));
    int arr[] = {1, 2, -3, 4};
    printf("%d\n", first_negative(arr, 4));
    printf("%d\n", digit_count(12345));
    printf("%d\n", digit_count(-99));
    return 0;
}