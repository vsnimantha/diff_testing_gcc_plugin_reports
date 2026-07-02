/* test_operators.c */
#include <stdio.h>
#include <stdlib.h>   /* required: makes abort() visible for plugin's lookup_name() */
#include <assert.h>

static void test_eq(int a, int b)                       { assert(a == b); }
static void test_ne(unsigned int a, unsigned int b)     { assert(a != b); }
static void test_lt(long a, long b)                     { assert(a < b); }
static void test_le(long unsigned int a, long unsigned int b) { assert(a <= b); }
static void test_gt(short a, short b)                   { assert(a > b); }
static void test_ge(int a, int b)                       { assert(a >= b); }

/* Same variable on both sides — exercises the once-per-decl guard:
   'x' appears twice but must only be printed once in subexpressions */
static void test_same_var(int x) {
    assert(x == x - 1);
}

int main(void) {
    test_eq(3, 3);
    test_ne(1u, 2u);
    test_lt(1L, 2L);
    test_le(2UL, 2UL);
    test_gt((short)5, (short)3);
    test_ge(4, 3);
    test_same_var(5);
    printf("all assertions passed\n");
    return 0;
}