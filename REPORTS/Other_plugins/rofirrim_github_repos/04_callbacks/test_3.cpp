
// test3_passes.cpp
#include <vector>

// Requesting inline forces the IPA (Interprocedural) passes to work harder
inline int compute(int x) {
    return x * x + 42;
}

int main() {
    long long total = 0;
    // A tight loop forces loop-unrolling and vectorization passes
    for (int i = 0; i < 10000; ++i) {
        total += compute(i);
    }
    
    return (total > 0) ? 0 : 1;
}