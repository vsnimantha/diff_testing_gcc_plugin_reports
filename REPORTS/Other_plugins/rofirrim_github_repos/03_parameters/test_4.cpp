// A template instantiation test
template <typename T>
T add(T a, T b) { return a + b; }

int main() {
    return add(2, 2) == 4 ? 0 : 1;
}