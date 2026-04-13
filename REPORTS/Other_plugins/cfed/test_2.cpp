/* test_linear.cpp
 *
 * arm-none-eabi-g++-7.3 -O1 -mcpu=cortex-m4 -mthumb -std=c++11
 *   -ffixed-r6 -ffixed-r10 -ffixed-r11 -fomit-frame-pointer
 *   -fno-jump-tables -fno-exceptions -fno-rtti
 *   -fplugin=./CFED_plugin64.so
 *   -fplugin-arg-CFED_plugin64-function=all
 *   -fplugin-arg-CFED_plugin64-techniqueType=SigMon
 *   -fplugin-arg-CFED_plugin64-techniqueSpecific=CFCSS
 *   -fplugin-arg-CFED_plugin64-selectiveLevel=0
 *   test_linear.cpp -o test_linear.elf
 *
 * What it covers:
 *   - plugin_init: function=all, SigMon, CFCSS, selectiveLevel=0
 *   - RTL pass: purely linear CFG — member functions on a plain
 *     struct produce branch-free basic block sequences. Tests that
 *     the plugin handles C++ name-mangled function names correctly
 *     when matching against the function= argument.
 *   - Template instantiation: two instantiations of the same template
 *     body produce two separately-named functions in the RTL, both
 *     of which the plugin must instrument independently.
 */

#include <cstdint>

extern "C" {
    void CFED_Detected(void){
        while(1);
    }
}

/* Plain struct with no virtual — no vtable, no hidden branches */
struct Accumulator {
    uint32_t value;

    explicit Accumulator(uint32_t init) : value(init) {}

    void add(uint32_t x)    { value += x; }
    void multiply(uint32_t x) { value *= x; }
    void mask(uint32_t m)   { value &= m; }
    uint32_t get() const    { return value; }
};

/* Function template — each instantiation becomes a separate RTL function */
template<typename T>
T saturate(T val, T lo, T hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

uint32_t checksum(const uint8_t *data, uint32_t len) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < len; ++i) {
        sum += data[i];
        sum ^= (sum << 3);
        sum += (sum >> 5);
    }
    return sum;
}

int main() {
    Accumulator acc(10);
    acc.add(5);
    acc.multiply(3);
    acc.mask(0xFF);

    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x08};
    uint32_t c = checksum(data, 5);

    uint32_t s = saturate<uint32_t>(c, 0u, 255u);
    int16_t  t = saturate<int16_t>(static_cast<int16_t>(acc.get()), -100, 100);

    return static_cast<int>((s + static_cast<uint32_t>(t)) & 0xFF);
}