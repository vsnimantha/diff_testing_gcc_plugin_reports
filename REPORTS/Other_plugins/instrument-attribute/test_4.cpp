
// test4_debug_and_combined.c
// Exercises:
//   1. Debug mode (PLUGIN_ARG: debug) -- triggers all the DEBUG() printf paths
//      inside handle(), should_instrument_function(), parse_plugin_args()
//   2. Combining file-list AND function-list simultaneously
//   3. The case where NO trigger matches (function is explicitly disabled)
//   4. Empty / edge-case list parsing in split_str (single-entry list,
//      list with a trailing comma which produces an empty token that
//      split_str must silently skip)
//
// Compile variants:
//
//   Variant A -- debug + file list (all functions instrumented, debug output):
//     gcc -fplugin=./plugin.so \
//         -fplugin-arg-plugin-debug \
//         -fplugin-arg-plugin-include-file-list=test4_debug_and_combined.c \
//         test4_debug_and_combined.c -o test4_varA
//
//   Variant B -- function list only, two names, trailing-comma edge case:
//     gcc -fplugin=./plugin.so \
//         -fplugin-arg-plugin-include-function-list=compute_gcd,compute_lcm, \
//         test4_debug_and_combined.c -o test4_varB
//
//   Variant C -- no plugin args: nothing instrumented, DECL_NO_INSTRUMENT set:
//     gcc -fplugin=./plugin.so \
//         test4_debug_and_combined.c -o test4_varC

#include <stdio.h>

// -- Utility helpers (not named in any list for variant B) --

static int abs_val(int x)
{
    return x < 0 ? -x : x;
}

// -- Functions targeted by the function-list in variant B --

int compute_gcd(int a, int b)
{
    a = abs_val(a);
    b = abs_val(b);
    while (b) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int compute_lcm(int a, int b)
{
    int g = compute_gcd(a, b);
    if (g == 0) return 0;
    return abs_val(a) / g * abs_val(b);
}

// -- More functions (instrumented in variant A via file list, not in B) --

int is_prime(int n)
{
    if (n < 2) return 0;
    for (int i = 2; (long)i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void print_primes_up_to(int limit)
{
    printf("Primes up to %d: ", limit);
    for (int i = 2; i <= limit; i++) {
        if (is_prime(i)) printf("%d ", i);
    }
    printf("\n");
}

int main(void)
{
    printf("gcd(48,18)  = %d\n", compute_gcd(48, 18));
    printf("lcm(4,6)    = %d\n", compute_lcm(4, 6));
    printf("gcd(-12,8)  = %d\n", compute_gcd(-12, 8));
    print_primes_up_to(30);
    return 0;
}