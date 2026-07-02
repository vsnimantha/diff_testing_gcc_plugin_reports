/*
 * test4_modern_cpp.cpp
 * ────────────────────
 * Coverage target: no_opt_attr_execute → happy path across modern C++
 * constructs that generate less obvious GIMPLE function bodies:
 * lambdas (each is a unique anonymous class with operator()), move
 * constructors/assignment operators, exception-handling paths, and
 * STL algorithm callbacks.
 *
 * Why this increases coverage:
 *   - Each lambda is compiled to a unique closure class whose
 *     operator() is a separate GIMPLE function body
 *   - Move constructors and move-assignment operators are distinct
 *     GIMPLE functions from their copy counterparts
 *   - Templated STL algorithms (sort, transform, accumulate) are
 *     instantiated with the lambda types, producing more bodies
 *   - try/catch blocks create extra CFG edges the GIMPLE pass visits
 *
 * Branches covered:
 *   plugin_init         : version match → register + return 0      [B]
 *   no_opt_attr_execute : no optimize attr → return 0              [D]
 *                         called on 30+ distinct GIMPLE bodies      [E]
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <stdexcept>
#include <memory>
#include <map>

// ── class with full rule-of-five ──────────────────────────────────
// Generates: default ctor, copy ctor, move ctor,
//            copy assign, move assign, destructor → 6 bodies

class StringBuffer {
    std::string data_;
    int         capacity_;
public:
    explicit StringBuffer(int cap = 64)
        : data_(), capacity_(cap) {}

    StringBuffer(const StringBuffer& o)
        : data_(o.data_), capacity_(o.capacity_) {}

    StringBuffer(StringBuffer&& o) noexcept
        : data_(std::move(o.data_)), capacity_(o.capacity_)
    { o.capacity_ = 0; }

    StringBuffer& operator=(const StringBuffer& o)
    {
        if (this != &o) { data_ = o.data_; capacity_ = o.capacity_; }
        return *this;
    }

    StringBuffer& operator=(StringBuffer&& o) noexcept
    {
        if (this != &o) {
            data_     = std::move(o.data_);
            capacity_ = o.capacity_;
            o.capacity_ = 0;
        }
        return *this;
    }

    ~StringBuffer() = default;

    void        append(const std::string& s) { data_ += s; }
    void        clear()                      { data_.clear(); }
    int         size()    const { return static_cast<int>(data_.size()); }
    int         capacity() const { return capacity_; }
    bool        empty()   const { return data_.empty(); }
    const std::string& str() const { return data_; }
};

// ── class with exception-throwing methods ─────────────────────────

class SafeArray {
    std::vector<int> data_;
public:
    explicit SafeArray(int size) : data_(size, 0) {}

    int& at(int i)
    {
        if (i < 0 || i >= static_cast<int>(data_.size()))
            throw std::out_of_range("SafeArray: index out of range");
        return data_[i];
    }

    int at(int i) const
    {
        if (i < 0 || i >= static_cast<int>(data_.size()))
            throw std::out_of_range("SafeArray: index out of range");
        return data_[i];
    }

    int  size()   const { return static_cast<int>(data_.size()); }
    void fill(int v)    { std::fill(data_.begin(), data_.end(), v); }

    int sum() const
    {
        return std::accumulate(data_.begin(), data_.end(), 0);
    }
};

// ── free function taking std::function ────────────────────────────

static int apply_twice(int x, const std::function<int(int)>& f)
{
    return f(f(x));
}

static std::vector<int> transform_vec(
    const std::vector<int>& v,
    const std::function<int(int)>& f)
{
    std::vector<int> out(v.size());
    std::transform(v.begin(), v.end(), out.begin(), f);
    return out;
}

// ── free functions with non-trivial CFG ──────────────────────────

static std::string classify(int n)
{
    if (n < 0)       return "negative";
    if (n == 0)      return "zero";
    if (n % 2 == 0)  return "positive even";
    return "positive odd";
}

static int safe_sqrt_floor(int n)
{
    if (n < 0) return -1;
    int i = 0;
    while ((i + 1) * (i + 1) <= n) ++i;
    return i;
}

// ── entry point ───────────────────────────────────────────────────

int main()
{
    // StringBuffer: exercises copy/move constructors
    StringBuffer sb1(128);
    sb1.append("hello");
    sb1.append(" world");
    StringBuffer sb2 = sb1;                 // copy ctor
    StringBuffer sb3 = std::move(sb1);      // move ctor
    sb2 = sb3;                              // copy assign
    sb3 = StringBuffer(64);                 // move assign
    std::cout << "sb2: " << sb2.str() << "  size=" << sb2.size() << '\n';

    // SafeArray with exception paths
    SafeArray arr(5);
    arr.fill(4);
    arr.at(0) = 10;
    arr.at(4) = 20;
    std::cout << "arr.sum()  = " << arr.sum() << '\n';

    try {
        arr.at(99);   // throws
    } catch (const std::out_of_range& e) {
        std::cout << "caught: " << e.what() << '\n';
    }

    // Lambdas — each lambda is a unique GIMPLE function body
    auto double_it  = [](int x)           { return x * 2; };
    auto add_ten    = [](int x)           { return x + 10; };
    auto is_even    = [](int x)           { return x % 2 == 0; };
    auto is_pos     = [](int x)           { return x > 0; };
    auto clamp_0_10 = [](int x)           { return x < 0 ? 0 : (x > 10 ? 10 : x); };

    // capturing lambda
    int factor = 3;
    auto multiply_by = [factor](int x)    { return x * factor; };

    // generic lambda (C++14) — each call with a different type = instantiation
    auto identity   = [](auto x)          { return x; };

    std::cout << "double_it(7)       = " << double_it(7)       << '\n';
    std::cout << "add_ten(5)         = " << add_ten(5)         << '\n';
    std::cout << "clamp_0_10(15)     = " << clamp_0_10(15)     << '\n';
    std::cout << "multiply_by(8)     = " << multiply_by(8)     << '\n';
    std::cout << "identity(42)       = " << identity(42)       << '\n';
    std::cout << "identity(3.14)     = " << identity(3.14)     << '\n';

    std::cout << "apply_twice(3)     = " << apply_twice(3, double_it) << '\n';

    // STL algorithms with lambdas — generates algorithm instantiations
    std::vector<int> v = {5, -3, 8, 1, -9, 2, 7, -4, 6};

    std::sort(v.begin(), v.end(), [](int a, int b){ return a < b; });
    std::cout << "sorted[0]          = " << v[0] << '\n';

    auto evens     = std::count_if(v.begin(), v.end(), is_even);
    auto positives = std::count_if(v.begin(), v.end(), is_pos);
    std::cout << "even count         = " << evens    << '\n';
    std::cout << "positive count     = " << positives << '\n';

    auto doubled = transform_vec(v, double_it);
    std::cout << "doubled sum        = "
              << std::accumulate(doubled.begin(), doubled.end(), 0) << '\n';

    // map with lambda comparator
    auto cmp = [](int a, int b){ return std::abs(a) < std::abs(b); };
    std::vector<int> v2 = v;
    std::sort(v2.begin(), v2.end(), cmp);
    std::cout << "abs-sorted[0]      = " << v2[0] << '\n';

    // CFG-heavy free functions
    for (int n : {-5, 0, 4, 7})
        std::cout << "classify(" << n << ")     = " << classify(n) << '\n';

    std::cout << "sqrt_floor(50)     = " << safe_sqrt_floor(50) << '\n';
    std::cout << "sqrt_floor(-1)     = " << safe_sqrt_floor(-1) << '\n';

    return 0;
}