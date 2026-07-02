
// test1_types_decl.cpp
struct SimpleStruct {
    int a;
    double b;
};

class ComplexClass {
private:
    SimpleStruct internal_data;
public:
    ComplexClass() { internal_data.a = 0; internal_data.b = 0.0; }
    void doNothing() {}
};

int main() {
    ComplexClass obj1;
    SimpleStruct obj2;
    int array[10] = {0};
    
    return array[0];
}