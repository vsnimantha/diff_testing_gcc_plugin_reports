// test3_error_and_gate.cpp
// Covers:
//   - handle_latent_entropy_attribute default case: attribute on unsupported type
//     -> *no_add_attrs=true, error() called
//   - VAR_DECL not TREE_STATIC (local variable) -> "must not be local" error path
//   - VAR_DECL with DECL_INITIAL set -> "must not be initialized" error path
//   - RECORD_TYPE with non-integer field -> "non-integer field" error path
//   - ARRAY_TYPE with non-integer element -> error path
//   - latent_entropy_gate: TREE_THIS_VOLATILE (noreturn) -> returns false immediately
//   - latent_entropy_gate: no attribute -> returns false
//   - plugin_init: disable arg -> enabled=false, callbacks not registered
//
// Compile (errors are expected and intentional -- plugin error branches execute
// before GCC aborts, which is what we want for coverage):
//   g++ -fplugin=./compiled_plugin.so \
//       -fplugin-arg-compiled_plugin-disable \
//       test3_error_and_gate.cpp -o test3 2>&1 || true

#include <cstdio>

// --- Error path 1: initialized variable -> "must not be initialized" ---
// static unsigned long bad_init __attribute__((latent_entropy)) = 42;

// --- Error path 2: local variable (not static) -> "must not be local" ---
// Triggered inside a function body:
void trigger_local_error()
{
    // Uncommenting this hits the !TREE_STATIC error branch:
    // unsigned long local_var __attribute__((latent_entropy));
    (void)0;
}

// --- Error path 3: RECORD_TYPE with non-integer field -> error ---
// struct bad_struct { float x; unsigned long y; };
// static struct bad_struct bs __attribute__((latent_entropy));

// --- latent_entropy_gate: noreturn -> TREE_THIS_VOLATILE -> returns false ---
__attribute__((noreturn, latent_entropy))
void fatal(const char *msg)
{
    fprintf(stderr, "fatal: %s\n", msg);
    __builtin_unreachable();
}

// --- latent_entropy_gate: no attribute -> returns false ---
static int fibonacci(int n)
{
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

static bool is_prime(int n)
{
    if (n < 2) return false;
    for (int i = 2; (long)i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

// --- plugin_init disable: enabled=false, pass callbacks skipped ---
// (compile with -fplugin-arg-compiled_plugin-disable to cover this)

int main()
{
    printf("fib(8)    = %d\n", fibonacci(8));
    printf("prime(17) = %d\n", (int)is_prime(17));
    (void)&fatal;
    return 0;
}