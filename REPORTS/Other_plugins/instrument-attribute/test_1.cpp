
// test1_attribute.c
// Exercises the __attribute__((instrument_function)) path directly.
// Every function decorated with the attribute should be instrumented;
// undecorated ones should be explicitly disabled.
//
// Compile with the plugin:
//   gcc -fplugin=./plugin.so test1_attribute.c -o test1_attribute

#include <stdio.h>

// Should be instrumented — has the attribute
__attribute__((instrument_function))
int add(int a, int b)
{
    return a + b;
}

// Should be instrumented — has the attribute
__attribute__((instrument_function))
int multiply(int a, int b)
{
    int result = 0;
    for (int i = 0; i < b; i++) {
        result = add(result, a);
    }
    return result;
}

// Should NOT be instrumented — no attribute
int subtract(int a, int b)
{
    return a - b;
}

// Should be instrumented — attribute on a void function
__attribute__((instrument_function))
void print_result(const char *label, int value)
{
    printf("%s = %d\n", label, value);
}

int main(void)
{
    int x = add(3, 4);
    int y = multiply(3, 4);
    int z = subtract(10, 3);
    print_result("add(3,4)", x);
    print_result("multiply(3,4)", y);
    print_result("subtract(10,3)", z);
    return 0;
}