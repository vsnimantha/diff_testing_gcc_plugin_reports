/*
 * sancov_test2.c — no-sancov arg: enable=false path
 * ───────────────────────────────────────────────────
 * Plain C.
 *
 * Plugin args required:
 *   -fplugin-arg-compiled_plugin-no-sancov
 *
 * plugin_init paths covered:
 *   - plugin_default_version_check pass
 *   - argv loop: key == "no-sancov" -> enable=false, continue  [B1]
 *   - register_callback PLUGIN_INFO
 *   - if (!enable) return 0                                     [D]
 *
 * The program itself is straightforward — plugin does nothing
 * when disabled, so program content only matters if execute() runs.
 * Include enough structure to cover all BBs if it does.
 */

#include <stdio.h>
#include <stdlib.h>

static int fib(int n)
{
    if (n <= 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int c = a + b;
        a = b;
        b = c;
    }
    return b;
}

static int is_prime(int n)
{
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

static void bubble_sort(int *arr, int n)
{
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j+1]) {
                int tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

static int binary_search(int *arr, int n, int target)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int main(void)
{
    for (int i = 0; i <= 10; i++)
        printf("fib(%d)=%d\n", i, fib(i));

    printf("primes:");
    for (int i = 2; i < 30; i++)
        if (is_prime(i)) printf(" %d", i);
    printf("\n");

    int arr[] = {5, 3, 8, 1, 9, 2, 7, 4, 6};
    int n = sizeof(arr) / sizeof(arr[0]);
    bubble_sort(arr, n);

    printf("sorted:");
    for (int i = 0; i < n; i++) printf(" %d", arr[i]);
    printf("\n");

    printf("search 7 at index %d\n", binary_search(arr, n, 7));
    printf("search 0 at index %d\n", binary_search(arr, n, 0));

    return 0;
}