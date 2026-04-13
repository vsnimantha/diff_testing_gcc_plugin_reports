// test4_tail_calls_and_cfg.cpp
// Compile:
//   g++ -fplugin=./compiled_plugin.so test4_tail_calls_and_cfg.cpp -o test4
//
// Unknown arg path:
//   g++ -fplugin=./compiled_plugin.so \
//       -fplugin-arg-compiled_plugin-unknownoption \
//       test4_tail_calls_and_cfg.cpp -o test4_err 2>&1 || true

#include <cstdio>

// Required global for plugin instrumentation
volatile unsigned long latent_entropy;

// Many branches -> many BBs -> perturb_local_entropy called per BB
__attribute__((latent_entropy))
static long classify(int x)
{
    if (x < 0)       return -1;
    if (x == 0)      return 0;
    if (x % 2 == 0)  return 2;
    if (x % 3 == 0)  return 3;
    if (x % 5 == 0)  return 5;
    if (x % 7 == 0)  return 7;
    return 1;
}

__attribute__((latent_entropy))
static long reduce(long *arr, int n, long init)
{
    long acc = init;
    for (int i = 0; i < n; i++) {
        if (arr[i] > 0)      acc += arr[i];
        else if (arr[i] < 0) acc -= arr[i];
        else                 acc ^= 0xDEADL;
    }
    return acc;
}

__attribute__((latent_entropy))
static int tail_add(int a, int b);

// Tail-call candidate -> gimple_call_tail_p true -> __perturb_latent_entropy
__attribute__((latent_entropy))
static int tail_wrapper(int a, int b)
{
    return tail_add(a + 1, b + 1);
}

__attribute__((latent_entropy))
static int tail_add(int a, int b)
{
    return a + b;
}

static void print_long(const char *label, long v)
{
    printf("%s = %ld\n", label, v);
}

int main()
{
    print_long("classify(-3)",  classify(-3));
    print_long("classify(0)",   classify(0));
    print_long("classify(6)",   classify(6));
    print_long("classify(35)",  classify(35));
    print_long("classify(11)",  classify(11));

    long data[] = {1, -2, 0, 3, -4, 5, 0, -6};
    print_long("reduce",        reduce(data, 8, 0));
    printf("tail_wrapper(10,20) = %d\n", tail_wrapper(10, 20));
    return 0;
}