/*
 * test2_classes.cpp
 * ─────────────────
 * Coverage target: no_opt_attr_execute → happy path across C++ class
 * constructs: constructors, destructors, virtual dispatch, operator
 * overloads, and inheritance chains.
 *
 * Each constructor, destructor, virtual method, and operator overload
 * is a separate GIMPLE function body — the plugin visits every one.
 * No optimize attributes are used so every invocation returns 0.
 *
 * Branches covered:
 *   plugin_init         : version match → register + return 0      [B]
 *   no_opt_attr_execute : no optimize attr → return 0              [D]
 *                         called on 25+ distinct GIMPLE bodies      [E]
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

// ── abstract base with virtual methods ───────────────────────────

class Shape {
public:
    explicit Shape(std::string name) : name_(std::move(name)) {}
    virtual ~Shape() = default;

    virtual double area()      const = 0;
    virtual double perimeter() const = 0;

    const std::string& name() const { return name_; }

    void print() const
    {
        std::cout << name_ << ": area=" << area()
                  << " perimeter=" << perimeter() << '\n';
    }

private:
    std::string name_;
};

// ── concrete subclasses ───────────────────────────────────────────

class Circle : public Shape {
    double r_;
public:
    explicit Circle(double r) : Shape("Circle"), r_(r) {}
    double area()      const override { return M_PI * r_ * r_; }
    double perimeter() const override { return 2.0 * M_PI * r_; }
    double radius()    const          { return r_; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : Shape("Rectangle"), w_(w), h_(h) {}
    double area()      const override { return w_ * h_; }
    double perimeter() const override { return 2.0 * (w_ + h_); }
    double aspect()    const          { return w_ / h_; }
};

class Triangle : public Shape {
    double a_, b_, c_;
public:
    Triangle(double a, double b, double c)
        : Shape("Triangle"), a_(a), b_(b), c_(c) {}
    double perimeter() const override { return a_ + b_ + c_; }
    double area() const override
    {
        double s = perimeter() / 2.0;
        return std::sqrt(s * (s-a_) * (s-b_) * (s-c_));
    }
};

// ── value class with operator overloads ──────────────────────────

class Vec2 {
public:
    double x, y;

    Vec2(double x = 0, double y = 0) : x(x), y(y) {}

    Vec2  operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2  operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2  operator*(double s)      const { return {x * s,   y * s};   }
    Vec2& operator+=(const Vec2& o)      { x += o.x; y += o.y; return *this; }
    bool  operator==(const Vec2& o) const { return x == o.x && y == o.y; }

    double dot(const Vec2& o)  const { return x*o.x + y*o.y; }
    double length()            const { return std::sqrt(x*x + y*y); }
    Vec2   normalised()        const
    {
        double len = length();
        return len > 0 ? Vec2{x/len, y/len} : Vec2{};
    }
};

// ── RAII wrapper class ────────────────────────────────────────────

class Buffer {
    std::vector<int> data_;
public:
    explicit Buffer(int size) : data_(size, 0) {}
    ~Buffer() = default;

    void   set(int i, int v)      { data_[i] = v; }
    int    get(int i)       const  { return data_[i]; }
    int    size()           const  { return static_cast<int>(data_.size()); }

    int sum() const
    {
        int total = 0;
        for (int v : data_) total += v;
        return total;
    }

    void fill(int v)
    {
        for (int& x : data_) x = v;
    }
};

// ── free helper ───────────────────────────────────────────────────

static double total_area(const std::vector<std::unique_ptr<Shape>>& shapes)
{
    double total = 0;
    for (const auto& s : shapes) total += s->area();
    return total;
}

// ── entry point ───────────────────────────────────────────────────

int main()
{
    // polymorphic dispatch — exercises vtable, ctors, dtors
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(3.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 5.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));

    for (const auto& s : shapes)
        s->print();

    std::cout << "total area = " << total_area(shapes) << '\n';

    // Vec2 operators
    Vec2 a{1.0, 2.0}, b{3.0, 4.0};
    Vec2 c = a + b;
    a += b;
    std::cout << "dot(a,b)   = " << a.dot(b)     << '\n';
    std::cout << "length(c)  = " << c.length()   << '\n';
    std::cout << "a==b       = " << (a == b)      << '\n';

    // Buffer RAII
    Buffer buf(8);
    buf.fill(3);
    buf.set(0, 10);
    std::cout << "buf.sum()  = " << buf.sum()     << '\n';
    std::cout << "buf.get(0) = " << buf.get(0)    << '\n';

    return 0;
}