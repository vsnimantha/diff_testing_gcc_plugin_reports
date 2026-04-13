// test4_redundant.c
// Triggers: gimple_assign_single_p, TREE_CODE(rhs1) == CONSTRUCTOR
struct target_struct {
    int __attribute__((user)) id;
};

int main() {
    // Already initialized! 
    // The plugin should detect this and NOT insert a second initializer.
    struct target_struct s1 = { .id = 0 }; 
    
    // Partially initialized or clobbered
    struct target_struct s2;
    s2.id = 1; // This is a partial assign, might still trigger full init depending on GCC version
    
    return 0;
}