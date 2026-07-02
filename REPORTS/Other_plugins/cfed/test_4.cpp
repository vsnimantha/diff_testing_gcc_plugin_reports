/* test_yacca.cpp
 *
 *   arm-none-eabi-g++-7.3 -O1 -mcpu=cortex-m4 -mthumb -std=c++11
 *     -ffixed-r6 -ffixed-r9 -ffixed-r10 -ffixed-r11
 *     -fomit-frame-pointer -fno-jump-tables
 *     -fno-exceptions -fno-rtti
 *     -fplugin=./CFED_plugin64.so
 *     -fplugin-arg-CFED_plugin64-function=all
 *     -fplugin-arg-CFED_plugin64-techniqueType=SigMon
 *     -fplugin-arg-CFED_plugin64-techniqueSpecific=YACCA
 *     -fplugin-arg-CFED_plugin64-selectiveLevel=0
 *     test_yacca.cpp -o test_yacca.elf
 *
 * What it covers:
 *   - YACCA path in plugin_init and RTL pass — 3 reserved registers,
 *     most register-intensive technique, exercises different RTL
 *     emission from single/dual register techniques.
 *   - Nested loops — inner loop creates a back-edge inside the outer
 *     loop body, so a basic block has both a fall-through predecessor
 *     and a back-edge predecessor simultaneously.
 *   - Early loop exit (break) — extra CFG edge from a mid-loop block
 *     to the post-loop merge block.
 *   - C++ template class — the compiler instantiates separate RTL
 *     functions for each template parameter, all of which the plugin
 *     instruments independently.
 *   - Recursive member function — exercises the second-stack push/pop
 *     across recursive calls with YACCA's 3-register bookkeeping.
 */

#include <cstdint>
#include <cstring>

extern "C" {
    void CFED_Detected(void){
        while(1);
    }
}


/* Template matrix class — two instantiations (int16_t and int32_t)
   produce two sets of separately-mangled RTL functions */
template<typename T, uint8_t N>
class Matrix {
    T data_[N * N];

public:
    Matrix() { memset(data_, 0, sizeof(data_)); }

    void set(uint8_t r, uint8_t c, T val) {
        data_[r * N + c] = val;
    }
    T get(uint8_t r, uint8_t c) const {
        return data_[r * N + c];
    }

    /* Nested loops — two back-edges in the CFG */
    void multiply(const Matrix<T, N> &b, Matrix<T, N> &out) const {
        for (uint8_t i = 0u; i < N; ++i) {
            for (uint8_t j = 0u; j < N; ++j) {
                T acc = 0;
                for (uint8_t k = 0u; k < N; ++k) {
                    acc += get(i, k) * b.get(k, j);
                }
                out.set(i, j, acc);
            }
        }
    }
};

/* Search with early break — extra exit edge from loop body */
static int16_t find_first_negative(const int16_t *arr, uint8_t len) {
    for (uint8_t i = 0u; i < len; ++i) {
        if (arr[i] < 0)
            return arr[i];  /* break creates an extra CFG edge */
    }
    return 0;
}

/* Mutually recursive pair — each call pushes/pops the second stack */
static uint32_t is_even(uint8_t n);
static uint32_t is_odd(uint8_t n);

static uint32_t is_even(uint8_t n) {
    if (n == 0u) return 1u;
    return is_odd(static_cast<uint8_t>(n - 1u));
}
static uint32_t is_odd(uint8_t n) {
    if (n == 0u) return 0u;
    return is_even(static_cast<uint8_t>(n - 1u));
}

int main() {
    Matrix<int16_t, 2> a, b, c;
    a.set(0, 0,  1); a.set(0, 1,  2);
    a.set(1, 0,  3); a.set(1, 1,  4);
    b.set(0, 0,  5); b.set(0, 1,  6);
    b.set(1, 0,  7); b.set(1, 1,  8);
    a.multiply(b, c);

    int16_t vals[] = { c.get(0,0), c.get(0,1),
                       c.get(1,0), c.get(1,1) };
    int16_t neg = find_first_negative(vals, 4u);

    uint32_t parity = is_even(7u);

    return static_cast<int>(
        (static_cast<uint32_t>(c.get(0,0)) +
         static_cast<uint32_t>(neg) +
         parity) & 0xFFu
    );
}