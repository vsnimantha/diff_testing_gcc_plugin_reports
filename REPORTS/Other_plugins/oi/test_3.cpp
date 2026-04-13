/*
 * test3_templates.cpp
 * ───────────────────
 * Coverage target: no_opt_attr_execute → happy path across C++
 * template constructs: function templates, class templates, explicit
 * specialisations, partial specialisations, and variadic templates.
 *
 * Each template instantiation is compiled to a separate GIMPLE
 * function body that the plugin visits independently.  A single
 * template instantiated with 4 different types gives 4 separate
 * no_opt_attr_execute calls.
 *
 * Branches covered:
 *   plugin_init         : version match → register + return 0      [B]
 *   no_opt_attr_execute : no optimize attr → return 0              [D]
 *                         called on 30+ distinct GIMPLE bodies      [E]
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <numeric>

// ── function templates (multiple instantiations) ──────────────────

template <typename T>
static T square(T x) { return x * x; }

template <typename T>
static T cube(T x) { return x * x * x; }

template <typename T>
static T clamp(T val, T lo, T hi)
{
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

template <typename T>
static T abs_val(T x) { return x < T{} ? -x : x; }

template <typename T>
static T lerp(T a, T b, double t)
{
    return static_cast<T>(a + (b - a) * t);
}

// ── explicit specialisation (separate GIMPLE body) ────────────────

template <>
std::string square<std::string>(std::string x) { return x + x; }

template <>
std::string cube<std::string>(std::string x) { return x + x + x; }

// ── class template with multiple methods ─────────────────────────

template <typename T>
class Stack {
    std::vector<T> data_;
public:
    Stack() = default;

    void push(const T& v) { data_.push_back(v); }

    T pop()
    {
        T v = data_.back();
        data_.pop_back();
        return v;
    }

    const T& top()   const { return data_.back(); }
    bool     empty() const { return data_.empty(); }
    int      size()  const { return static_cast<int>(data_.size()); }

    T sum() const
    {
        T acc{};
        for (const T& v : data_) acc += v;
        return acc;
    }
};

// ── class template partial specialisation (pointer types) ─────────

template <typename T>
class Wrapper {
    T value_;
public:
    explicit Wrapper(T v) : value_(v) {}
    T       get()   const { return value_; }
    void    set(T v)      { value_ = v; }
    bool    operator==(const Wrapper& o) const { return value_ == o.value_; }
};

template <typename T>
class Wrapper<T*> {
    T* ptr_;
public:
    explicit Wrapper(T* p) : ptr_(p) {}
    T*   get()   const { return ptr_; }
    bool is_null() const { return ptr_ == nullptr; }
};

// ── variadic template (pack expansion generates multiple bodies) ──

template <typename T>
static T variadic_sum(T v) { return v; }

template <typename T, typename... Ts>
static T variadic_sum(T first, Ts... rest)
{
    return first + variadic_sum(rest...);
}

template <typename T>
static void print_all(T v) { std::cout << v << '\n'; }

template <typename T, typename... Ts>
static void print_all(T first, Ts... rest)
{
    std::cout << first << ' ';
    print_all(rest...);
}

// ── constexpr functions ────────────────────────────────────────────

constexpr int triangle_number(int n)  { return n * (n + 1) / 2; }
constexpr int fibonacci_ct(int n)
{
    return n <= 1 ? n : fibonacci_ct(n-1) + fibonacci_ct(n-2);
}
constexpr double celsius_to_f(double c) { return c * 9.0/5.0 + 32.0; }

// ── entry point ───────────────────────────────────────────────────

int main()
{
    // function template instantiations: int, double, float, long
    std::cout << "square<int>(5)     = " << square<int>(5)           << '\n';
    std::cout << "square<double>(2.5)= " << square<double>(2.5)      << '\n';
    std::cout << "square<float>(3.f) = " << square<float>(3.0f)      << '\n';
    std::cout << "square<string>     = " << square<std::string>("ab") << '\n';

    std::cout << "cube<int>(3)       = " << cube<int>(3)             << '\n';
    std::cout << "cube<double>(2.0)  = " << cube<double>(2.0)        << '\n';
    std::cout << "cube<string>       = " << cube<std::string>("x")   << '\n';

    std::cout << "clamp<int>         = " << clamp<int>(15, 0, 10)    << '\n';
    std::cout << "clamp<double>      = " << clamp<double>(1.5, 0.0, 1.0) << '\n';

    std::cout << "abs_val<int>(-7)   = " << abs_val<int>(-7)         << '\n';
    std::cout << "abs_val<float>     = " << abs_val<float>(-3.5f)    << '\n';

    std::cout << "lerp<int>(0,100,.3)= " << lerp<int>(0, 100, 0.3)  << '\n';

    // class template: Stack<int> and Stack<std::string>
    Stack<int> si;
    si.push(1); si.push(2); si.push(3);
    std::cout << "Stack<int>.sum     = " << si.sum()   << '\n';
    std::cout << "Stack<int>.pop     = " << si.pop()   << '\n';

    Stack<std::string> ss;
    ss.push("hello"); ss.push(" "); ss.push("world");
    std::cout << "Stack<str>.size    = " << ss.size()  << '\n';
    std::cout << "Stack<str>.top     = " << ss.top()   << '\n';

    // Wrapper value and pointer specialisations
    Wrapper<int>  wi(42);
    int           val = 7;
    Wrapper<int*> wp(&val);
    std::cout << "Wrapper<int>.get   = " << wi.get()      << '\n';
    std::cout << "Wrapper<int*>.null = " << wp.is_null()  << '\n';

    // variadic templates
    std::cout << "variadic_sum       = " << variadic_sum(1, 2, 3, 4, 5) << '\n';
    print_all(10, 20, 30, 40);

    // constexpr
    std::cout << "triangle_number(10)= " << triangle_number(10) << '\n';
    std::cout << "fibonacci_ct(10)   = " << fibonacci_ct(10)    << '\n';
    std::cout << "32C in F           = " << celsius_to_f(100.0) << '\n';

    return 0;
}