
// test3_function_list.c
// Exercises the include-function-list plugin argument path.
// Only the explicitly named functions should be instrumented.
//
// Compile:
//   gcc -fplugin=./plugin.so \
//       -fplugin-arg-plugin-include-function-list=fibonacci,factorial \
//       test3_function_list.c -o test3_function_list
//
// fibonacci and factorial -> instrumented
// helper_square, main     -> NOT instrumented

#include <stdio.h>

// NOT in function list -- should not be instrumented
int helper_square(int n)
{
    return n * n;
}

// IN function list -- should be instrumented
long factorial(int n)
{
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// IN function list -- should be instrumented
long fibonacci(int n)
{
    if (n <= 0) return 0;
    if (n == 1) return 1;
    long a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        long tmp = a + b;
        a = b;
        b = tmp;
    }
    return b;
}

// NOT in function list -- should not be instrumented
void print_table(int max)
{
    for (int i = 0; i <= max; i++) {
        printf("  fib(%2d)=%6ld   %2d!=%10ld   sq(%2d)=%4d\n",
               i, fibonacci(i), i, factorial(i), i, helper_square(i));
    }
}

int main(void)
{
    print_table(10);
    return 0;
}