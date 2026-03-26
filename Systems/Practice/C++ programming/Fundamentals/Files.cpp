#include <iostream>
#include <fstream>
#include <ctime>

void fileCreation()
{
    std::ofstream file1("Text.txt");

    file1 << "Hello world\n";

    file1.close();
}

void readFile(std::string fileName)
{
    std::ifstream file2(fileName);
    std::string fileBuffer;

    while (getline(file2, fileBuffer))
    {
        std::cout << fileBuffer;
    }

    file2.close();
}

void showDate()
{
    time_t timeStamp = time(&timeStamp);

    std::cout << ctime(&timeStamp);
}

int main(int args, char *argv[])
{
    readFile("Text.txt");
    std::cout << "\n";
    showDate();
    return 0;
}