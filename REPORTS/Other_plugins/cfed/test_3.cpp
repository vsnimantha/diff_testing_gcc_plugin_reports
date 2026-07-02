/* test_branches.cpp
 *
 * Run A — fullCFED, RACFED, all blocks:
 *   ... -fplugin-arg-CFED_plugin64-techniqueType=fullCFED
 *       -fplugin-arg-CFED_plugin64-techniqueSpecific=RACFED
 *       -fplugin-arg-CFED_plugin64-selectiveLevel=0
 *       -fplugin-arg-CFED_plugin64-function=all
 *
 * Run B — SigMon, exit blocks only:
 *   ... -fplugin-arg-CFED_plugin64-techniqueType=SigMon
 *       -fplugin-arg-CFED_plugin64-techniqueSpecific=RACFED
 *       -fplugin-arg-CFED_plugin64-selectiveLevel=1
 *       -fplugin-arg-CFED_plugin64-function=all
 *
 * What it covers:
 *   - C++ enum class — same integer semantics as C enum but scoped,
 *     exercises the plugin on switch-over-enum CFGs.
 *   - Multiple return points — 7+ exit blocks, stressing the
 *     selectiveLevel=1 path that only inserts comparisons at exits.
 *   - fullCFED mode for RACFED (intra-block + inter-block).
 *   - A class method with a non-trivial CFG — the plugin must walk
 *     the CFG of a member function whose 'this' pointer is in a register.
 */

#include <cstdint>

extern "C" {
    void CFED_Detected(void){
        while(1);
    }
}

enum class State : uint8_t {
    Idle   = 0,
    Active = 1,
    Error  = 2,
    Done   = 3
};

class Controller {
    uint32_t accumulator_;
    uint8_t  flags_;

public:
    Controller(uint32_t init, uint8_t flags)
        : accumulator_(init), flags_(flags) {}

    /* Complex CFG — multiple conditional branches, multiple returns */
    int32_t process(State state, int32_t value) {
        if (state == State::Idle) {
            if (flags_ & 0x01)
                return -1;
            return 0;
        }
        if (state == State::Error)
            return -2;

        if (state == State::Done) {
            if (value > 100)      return value - 100;
            else if (value < 0)   return 0;
            else                  return value;
        }

        /* State::Active — longer path */
        int32_t result = static_cast<int32_t>(accumulator_);
        if (flags_ & 0x02) result *= 2;
        if (flags_ & 0x04) result += 10;
        if (flags_ & 0x08) result ^= 0xAA;
        if (flags_ & 0x10) result >>= 1;
        return result;
    }

    uint32_t dispatch(uint8_t cmd, uint32_t arg) {
        switch (cmd) {
            case 0:  return arg + 1;
            case 1:  return arg * 2;
            case 2:  return arg ^ 0xFFu;
            case 3:  return arg >> 2;
            case 4:  return arg << 1;
            default: return 0u;
        }
    }

    uint32_t get() const { return accumulator_; }
};

/* Free function with deep nested conditionals — stresses D-value
   computation at every CFG merge point */
static uint32_t classify(uint32_t x, uint32_t y, uint32_t z) {
    uint32_t result = 0;
    if (x > 50) {
        if (y > 50) {
            result = x + y;
        } else {
            result = x - y;
            if (z > 10) result += z;
        }
    } else {
        if (z > 100) {
            result = z;
        } else {
            result = x * 2 + y;
        }
    }
    return result & 0xFFu;
}

int main() {
    Controller ctrl(42u, 0x07u);
    int32_t r1 = ctrl.process(State::Active, 42);
    int32_t r2 = ctrl.process(State::Done,  150);
    int32_t r3 = ctrl.process(State::Error,   0);
    uint32_t d = ctrl.dispatch(2u, static_cast<uint32_t>(r1));
    uint32_t c = classify(60u, 40u, 15u);
    return static_cast<int>((r1 + r2 + r3 + static_cast<int32_t>(d + c)) & 0xFF);
}