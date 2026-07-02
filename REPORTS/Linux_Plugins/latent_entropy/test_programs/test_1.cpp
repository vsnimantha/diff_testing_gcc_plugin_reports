// test1_integer_and_func.cpp
// Compile:
//   g++ -fplugin=./compiled_plugin.so test1_integer_and_func.cpp -o test1

#include <cstdio>

// Required: the plugin injects reads/writes of this global symbol
// into every function marked __attribute__((latent_entropy)).
volatile unsigned long latent_entropy;

// static file-scope, no initializer, unsigned -> INTEGER_TYPE unsigned branch
static unsigned long entropy_uint __attribute__((latent_entropy));

// static file-scope, signed -> INTEGER_TYPE signed branch
static long entropy_sint __attribute__((latent_entropy));

// Attributed functions -> FUNCTION_DECL + gate true -> full instrumentation
__attribute__((latent_entropy))
int compute_sum(int n)
{
    int s = 0;
    for (int i = 1; i <= n; i++) s += i;
    return s;
}

__attribute__((latent_entropy))
int compute_product(int a, int b)
{
    int r = 1;
    for (int i = 0; i < b; i++) r *= a;
    return r;
}

// No attribute -> gate returns false
static int helper(int x) { return x * x; }

int main()
{
    printf("entropy_uint   = %lu\n", entropy_uint);
    printf("entropy_sint   = %ld\n", entropy_sint);
    printf("sum(10)        = %d\n",  compute_sum(10));
    printf("product(3,4)   = %d\n",  compute_product(3, 4));
    printf("helper(7)      = %d\n",  helper(7));
    return 0;
}