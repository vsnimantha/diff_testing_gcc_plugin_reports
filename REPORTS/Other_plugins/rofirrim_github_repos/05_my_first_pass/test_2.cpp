
// test2_methods.cpp
class Calculator {
public:
    Calculator() {} // Trigger 1
    int add(int a, int b) { return a + b; } // Trigger 2
    ~Calculator() {} // Trigger 3
};

int main() {
    Calculator calc;
    return calc.add(5, 5);
}