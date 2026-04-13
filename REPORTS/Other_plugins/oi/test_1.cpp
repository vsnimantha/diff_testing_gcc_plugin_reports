/*
 * test1_baseline.cpp
 * ──────────────────
 * Coverage target: no_opt_attr_execute → happy path (return 0)
 *
 * C++ version: uses classes, templates, lambdas, operator overloads,
 * and member functions so the plugin visits many more GIMPLE functions
 * (constructors, destructors, and template instantiations each count
 * as separate function bodies).
 *
 * Branches covered:
 *   plugin_init         : version match → register + return 0      [B]
 *   no_opt_attr_execute : no optimize attr → return 0              [D]
 *                         called on 20+ distinct functions          [E]
 */

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

// ── free functions ────────────────────────────────────────────────

static int add(int a, int b)      { return a + b; }
static int subtract(int a, int b) { return a - b; }
static int multiply(int a, int b) { return a * b; }

static int max_of_two(int a, int b)
{
    return a > b ? a : b;
}

static int clamp(int val, int lo, int hi)
{
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

static int fibonacci(int n)
{
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// ── function template (each instantiation = separate GIMPLE body) ──

template <typename T>
static T square(T x) { return x * x; }

template <typename T>
static T cube(T x) { return x * x * x; }

// ── class with constructor, destructor, and methods ───────────────

class Counter {
    int value_;
public:
    explicit Counter(int start = 0) : value_(start) {}
    ~Counter() = default;

    void increment()       { ++value_; }
    void decrement()       { --value_; }
    void add(int n)        { value_ += n; }
    int  get() const       { return value_; }
    void reset()           { value_ = 0; }

    Counter& operator+=(int n) { value_ += n; return *this; }
    bool     operator==(const Counter& o) const { return value_ == o.value_; }
};

// ── class template ────────────────────────────────────────────────

template <typename T>
class Pair {
    T first_, second_;
public:
    Pair(T a, T b) : first_(a), second_(b) {}
    T   first()  const { return first_; }
    T   second() const { return second_; }
    T   sum()    const { return first_ + second_; }
    Pair<T> swap_vals() const { return Pair<T>(second_, first_); }
};

// ── range helper using STL ────────────────────────────────────────

static int sum_vector(const std::vector<int>& v)
{
    return std::accumulate(v.begin(), v.end(), 0);
}

static int count_evens(const std::vector<int>& v)
{
    return static_cast<int>(
        std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; })
    );
}

// ── entry point ───────────────────────────────────────────────────

int main()
{
    std::cout << "add(3,4)        = " << add(3, 4)        << '\n';
    std::cout << "subtract(9,4)   = " << subtract(9, 4)   << '\n';
    std::cout << "multiply(6,7)   = " << multiply(6, 7)   << '\n';
    std::cout << "max(11,7)       = " << max_of_two(11, 7) << '\n';
    std::cout << "clamp(15,0,10)  = " << clamp(15, 0, 10) << '\n';
    std::cout << "fib(8)          = " << fibonacci(8)      << '\n';
    std::cout << "square<int>(9)  = " << square<int>(9)    << '\n';
    std::cout << "square<double>  = " << square<double>(2.5) << '\n';
    std::cout << "cube<int>(3)    = " << cube<int>(3)      << '\n';

    Counter c(10);
    c.increment();
    c.add(5);
    c += 3;
    std::cout << "Counter         = " << c.get() << '\n';

    Pair<int>    pi(3, 7);
    Pair<double> pd(1.5, 2.5);
    std::cout << "Pair<int>.sum   = " << pi.sum()         << '\n';
    std::cout << "Pair<double>.sum= " << pd.sum()         << '\n';

    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::cout << "sum_vector      = " << sum_vector(v)    << '\n';
    std::cout << "count_evens     = " << count_evens(v)   << '\n';

    return 0;
}