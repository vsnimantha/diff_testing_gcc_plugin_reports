// test2_struct_and_array.cpp
// Compile:
//   g++ -fplugin=./compiled_plugin.so test2_struct_and_array.cpp -o test2

#include <cstdio>

// Required global for plugin instrumentation
volatile unsigned long latent_entropy;

// ARRAY_TYPE: fixed-size unsigned long array, static, no initializer
// -> nelt=4, calls tree_get_random_const 4 times, exercises vec_alloc + loop
static unsigned long entropy_pool[4] __attribute__((latent_entropy));

// ARRAY_TYPE: different size to exercise the nelt calculation again
static unsigned long entropy_pool8[8] __attribute__((latent_entropy));

// INTEGER_TYPE unsigned -> build_int_cstu
static unsigned long entropy_a __attribute__((latent_entropy));

// INTEGER_TYPE signed -> build_int_cst
static long entropy_b __attribute__((latent_entropy));

// Attributed functions to trigger latent_entropy_execute
__attribute__((latent_entropy))
static unsigned long mix(unsigned long x, unsigned long y)
{
    return x ^ (y << 3) ^ (y >> 5);
}

__attribute__((latent_entropy))
static unsigned long fold(unsigned long *arr, int n)
{
    unsigned long acc = 0;
    for (int i = 0; i < n; i++) acc = mix(acc, arr[i]);
    return acc;
}

static void print_hex(const char *label, unsigned long v)
{
    printf("%s = 0x%lx\n", label, v);
}

int main()
{
    unsigned long data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    print_hex("fold",        fold(data, 8));
    print_hex("mix",         mix(0xAB, 0xCD));
    print_hex("entropy_a",   entropy_a);
    print_hex("entropy_b",   (unsigned long)entropy_b);
    print_hex("pool[0]",     entropy_pool[0]);
    print_hex("pool8[0]",    entropy_pool8[0]);
    return 0;
}