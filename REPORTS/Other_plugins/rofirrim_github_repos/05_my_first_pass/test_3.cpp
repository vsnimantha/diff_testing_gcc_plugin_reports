
// test3_templates.cpp
template <typename T>
T square(T val) {
    return val * val;
}

int main() {
    int a = square<int>(5);        // Generates square<int>
    double b = square<double>(5.5); // Generates square<double>
    return 0;
}