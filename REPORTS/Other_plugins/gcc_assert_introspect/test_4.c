/* test_calls_and_scopes.c */
#include <stdio.h>
#include <stdlib.h>   /* required: abort() must be visible */
#include <string.h>
#include <assert.h>

static int add(int a, int b)              { return a + b; }
static int multiply(int a, int b)         { return a * b; }
static int clamp(int v, int lo, int hi)   { return v < lo ? lo : v > hi ? hi : v; }

/* Single CALL_EXPR in assert — make_call_subexpression_repr */
static void test_call_in_assert(int a, int b, int expected) {
    assert(add(a, b) == expected);
}

/* Two CALL_EXPRs with && — TRUTH_ANDIF + two call reprs */
static void test_two_calls(int a, int b, int c, int d) {
    assert(add(a, b) > 0 && multiply(c, d) > 0);
}

/* Multi-arg call — all 3 args appear in subexpressions repr */
static void test_multiarg_call(int val) {
    assert(clamp(val, 0, 100) == val);
}

/* strcmp — standard library call via get_callee_fndecl */
static void test_strcmp_in_assert(const char *a, const char *b) {
    assert(strcmp(a, b) == 0);
}

/* Nested BIND_EXPR — iterate_function_body must recurse into inner scope */
static void test_assert_in_nested_scope(int x, int limit) {
    if (x > 0) {
        assert(x < limit);   /* inside nested BIND_EXPR */
    }
    {
        int local = x * 2;
        assert(local >= 0);  /* inside a bare block */
    }
}

/* Assert inside for loop — another nested BIND_EXPR */
static void test_assert_in_loop(int *arr, int n) {
    int i;
    for (i = 0; i < n; i++) {
        assert(arr[i] >= 0);
    }
}

/* Multiple asserts in sequence — make_assert_expr_printf called for
   each, exercising file/line extraction for different line numbers */
static void test_multiple_asserts(int a, int b, int c) {
    assert(a >= 0);
    assert(b >= 0);
    assert(c >= 0);
    assert(a + b + c < 1000);
}

int main(void) {
    int arr[] = {1, 2, 3, 4, 5};

    test_call_in_assert(2, 3, 5);
    test_two_calls(1, 2, 3, 4);
    test_multiarg_call(50);
    test_strcmp_in_assert("hello", "hello");
    test_assert_in_nested_scope(3, 10);
    test_assert_in_loop(arr, 5);
    test_multiple_asserts(1, 2, 3);

    printf("all assertions passed\n");
    return 0;
}