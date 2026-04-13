
// test2_pragmas_attrs.cpp

// Force a pragma to be read by the frontend
#pragma GCC optimize ("O3")
#pragma message "Compiling test 2..."

// Force an attribute to be parsed
struct __attribute__((packed)) PackedStruct {
    char c;
    int i;
};

int main() {
    int aligned_var __attribute__((aligned(16))) = 42;
    PackedStruct ps;
    ps.c = 'A';
    
    return (aligned_var == 42) ? 0 : 1;
}