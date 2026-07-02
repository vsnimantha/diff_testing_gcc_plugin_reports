// test2_nested.c
// Triggers: is_userspace_type (recursion), get_field_type
struct leaf {
    int __attribute__((user)) data;
};

struct branch {
    struct leaf inner;
};

struct root {
    struct branch mid;
};

int main() {
    // The plugin must recurse through root -> branch -> leaf to find 'user'
    struct root nest;
    return 0;
}