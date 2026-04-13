/* test_types_and_pointers.c */
#include <stdio.h>
#include <stdlib.h>   /* required: abort() must be visible */
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Pointer type — get_format_for_expr → %p */
static void test_pointer(int *p, int *q)         { assert(p == q); }

/* NULL check — is_NULL path */
static void test_null_pointer(int *p)            { assert(p != NULL); }

/* String pointer — TYPE_STRING_FLAG → format "%s" */
static void test_string(const char *s, const char *t) { assert(s == t); }

/* String literal on rhs — ADDR_EXPR with STRING_CST */
static void test_string_literal(const char *name) {
    assert(name != "forbidden");
}

/* Float type — get_real_type_name → "float", format %f */
static void test_float(float a, float b)         { assert(a == b); }

/* Double type */
static void test_double(double a, double b)      { assert(a < b); }

/* Long double — format %Lf */
static void test_long_double(long double a, long double b) { assert(a <= b); }

/* Boolean — TREE_CODE(type) == BOOLEAN_TYPE → %d */
static void test_bool(bool flag)                 { assert(flag == true); }

/* Bitwise operators */
static void test_bitor(unsigned int a, unsigned int b, unsigned int e)  { assert((a | b) == e); }
static void test_bitxor(unsigned int a, unsigned int b, unsigned int e) { assert((a ^ b) == e); }
static void test_bitand(unsigned int a, unsigned int b, unsigned int e) { assert((a & b) == e); }

/* ADDR_EXPR with DECL_P inner — &variable in assert */
static void test_addr_of_var(int *expected) {
    int local = 42;
    assert(&local == expected);
}

int main(void) {
    int x = 5, y = 5;
    test_pointer(&x, &x);
    test_null_pointer(&x);
    test_string("hello", "hello");
    test_string_literal("allowed");
    test_float(1.5f, 1.5f);
    test_double(1.0, 2.0);
    test_long_double(1.0L, 1.0L);
    test_bool(true);
    test_bitor(0xA0u, 0x0Bu, 0xABu);
    test_bitxor(0xFFu, 0x0Fu, 0xF0u);
    test_bitand(0xABu, 0x0Fu, 0x0Bu);
    test_addr_of_var(&x);
    printf("all assertions passed\n");
    return 0;
}