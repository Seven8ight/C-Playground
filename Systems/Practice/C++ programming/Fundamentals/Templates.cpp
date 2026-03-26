#include <iostream>

template <typename T>
auto add(T a, T b)
{
    return a + b;
}

template <typename F, typename F2>
class Box
{
public:
    F one;
    F2 two;

    Box(F cOne, F2 cTwo)
    {
        this->one = cOne;
        this->two = cTwo;
    }

    void info()
    {
        std::cout << "First: " << this->one << "\n";
        std::cout << "Two: " << this->two << "\n";
    }
};

int main(int argc, char *argv[])
{
    std::cout << add<int>(1, 2) << "\n";
    std::cout << add<double>(1, 2) << "\n";
}