/* test_p4_minimal.c */
typedef void (*fn_t)(void);

fn_t global_fn_ptr = (fn_t)0;

static void task_a(void) { volatile int x = 1; (void)x; }

static void store_to_global(fn_t enc) {
    global_fn_ptr = enc;
}

int main(void) {
    store_to_global(task_a);
    return 0;
}