#include <iostream>

int main()
{
    int number;
    std::cout << "Enter a number: ";
    while (!(std::cin >> number))
    { // Keep asking until the user enters a valid number
        std::cout << "Invalid input. Try again: ";
        std::cin.clear();             // Reset input errors
        std::cin.ignore(10000, '\n'); // Remove bad input
    }
    std::cout << "You entered: " << number;
    return 0;
}