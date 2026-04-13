
// test4_genericize.cpp
template <typename T>
T findMax(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    int intMax = findMax<int>(10, 20);
    double doubleMax = findMax<double>(3.14, 6.28);
    
    return (intMax == 20) ? 0 : 1;
}