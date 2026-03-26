#include <iostream>
#include <vector>

template <typename F>
std::vector<std::vector<F>> createMatrix(int rows, int columns)
{
    return std::vector<std::vector<F>>(rows, std::vector<F>(columns, 0));
}

template <typename F>
void addEdge(std::vector<std::vector<F>> &matrix, int row, int column)
{
    matrix.at(row).at(column) = 1;
}

template <typename F>
void displayMatrix(std::vector<std::vector<F>> matrix)
{
    int rowCount = 1;
    for (const auto &row : matrix)
    {
        std::cout << "Row " << rowCount << ": ";
        int j = 0;

        for (const auto &value : row)
        {
            if (j != 2)
                std::cout << value << ", ";
            else
                std::cout << value;

            j++;
        }

        std::cout << std::endl;
        rowCount++;
    }
}

int main()
{
    auto matrix = createMatrix<int>(10, 3);

    addEdge(matrix, 1, 2);
    addEdge(matrix, 4, 2);

    displayMatrix(matrix);

    return 0;
}