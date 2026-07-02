// test1_attr.c
// Triggers: handle_user_attribute, is_userspace_type, finish_type
struct target_struct {
    int __attribute__((user)) secret_key;
    int public_data;
};

void test_func() {
    // This local variable should be forcibly initialized by the plugin
    struct target_struct leaked_data;
    // We don't initialize it here to trigger the plugin's insertion logic
}

int main() {
    test_func();
    return 0;
}