#include <iostream>
#include <functional>

#define max(a, b) (((a) > (b) ? (a) : (b)))

typedef enum Values
{
    TRUE,
    FALSE
} Values;

// auto *funct(FILE *file)
// {
//     if (!file)
//     {
//         std::cout << "File is empty/corrupted\n";
//         return "NULL";
//     }

//     return;
// }

void changeValue(int &num)
{
    num = 10;
}

void countdown(int k)
{
    if (k != 0 || k != 1)
    {
        std::cout << k << "\n";
        countdown(k - 1);
    }
}

int factorial(int n)
{
    if (n == 0 || n == 1)
        return n;
    return n * factorial(n - 1);
}

void lambdaFunctions()
{
    auto func = []()
    {
        std::cout << "Hello world, lambda functions\n";
    };

    func();
}

void lamdbaUsage(std::function<void()> func)
{
    func();
    func();
}

void dynamicMem()
{
    int *dynamic = new int;
    std::string *name = new std::string;

    *dynamic = 5;
    *name = "Lawrence";

    printf("%d\n", *dynamic);

    std::cout << &name << "\n";
    std::cout << name << ":" << *name << "\n";

    std::cout << "Sizeof int: "
              << sizeof dynamic << " bytes\n";
    std::cout << "Sizeof string: "
              << sizeof name << " bytes\n";

    delete dynamic;
    delete name;
}

int main(int argc, char *args[])
{
    std::vector<std::string> names;

    std::string firstName = "Lawrence",
                lastName = "Muchiri",
                fullName = firstName.append(" " + lastName);

    for (int i = 0; i < fullName.length(); i++)
        std::cout << fullName[i] << " ";

    std::cout << "\n";

    bool value = false;

    auto toString = [](int value)
    {
        return std::to_string(value);
    };

    std::string valueToString = toString(100);

    std::cout << "Integer to string: " << valueToString << "\n";

    // std::string input;
    // std::cout << "Enter your name: ";
    // getline(std::cin, input);

    // std::cout << "\nFull name: " << input;

    std::cout << std::boolalpha;

    std::cout << value << "\n";

    std::cout << std::noboolalpha;

    int ages[] = {12, 23, 54, 64, 42};

    for (int age : ages)
    {
        if (age > 18)
            std::cout << "An adult\n";
        else
            std::cout << "A child still\n";

        auto output = [age]()
        {
            std::cout << age << "\n";
        };

        output();
    }

    dynamicMem();
    return 0;
}