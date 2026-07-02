/* test_logical_arith.c */
#include <stdio.h>
#include <stdlib.h>   /* required: abort() must be visible */
#include <assert.h>

/* TRUTH_ANDIF_EXPR — left fails → print left only */
static void test_and(int a, int b, int c) {
    assert(a > 0 && b > 0 && c > 0);
}

/* TRUTH_ORIF_EXPR — both fail → print both sides */
static void test_or(int a, int b) {
    assert(a != 0 || b != 0);
}

/* Mixed && and || — nested logical repr tree */
static void test_mixed_logical(int x, int y, int z) {
    assert((x > 0 && y > 0) || z > 0);
}

/* Arithmetic operators in assert condition */
static void test_arith_plus(int a, int b, int expected)  { assert(a + b == expected); }
static void test_arith_minus(int a, int b, int expected) { assert(a - b == expected); }
static void test_arith_mult(int a, int b, int expected)  { assert(a * b == expected); }
static void test_arith_div(int a, int b, int expected)   { assert(a / b == expected); }

/* % operator — exercises the %% escape in make_conditional_expr_repr */
static void test_arith_mod(int a, int b, int expected)   { assert(a % b == expected); }

int main(void) {
    test_and(1, 2, 3);
    test_or(0, 1);
    test_mixed_logical(1, 2, 0);
    test_arith_plus(2, 3, 5);
    test_arith_minus(5, 3, 2);
    test_arith_mult(3, 4, 12);
    test_arith_div(10, 2, 5);
    test_arith_mod(7, 3, 1);
    printf("all assertions passed\n");
    return 0;
}