// test3_classes.cpp
class Shape {
public:
    virtual int getArea() = 0;
    virtual ~Shape() {}
};

class Rectangle : public Shape {
private:
    int width;
    int height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    int getArea() override {
        return width * height;
    }
};

int main() {
    Rectangle rect(5, 5);
    Shape* shape = &rect;
    
    if (shape->getArea() == 25) {
        return 0;
    }
    return 1;
}