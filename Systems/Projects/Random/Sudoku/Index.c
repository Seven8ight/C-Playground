#include "Header.h"

/* ================= DISPLAY ================= */

void display(Sudoku *s)
{
    printf("\n");

    for (int i = 0; i < SIZE; i++)
    {
        if (i % 3 == 0)
            printf("+-------+-------+-------+\n");

        for (int j = 0; j < SIZE; j++)
        {
            if (j % 3 == 0)
                printf("| ");

            if (s->board[i][j] == 0)
                printf(". ");
            else
                printf("%d ", s->board[i][j]);
        }

        printf("|\n");
    }

    printf("+-------+-------+-------+\n");
}

/* ================= VALIDATION ================= */

bool isSafe(Sudoku *s, int row, int col, int num)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (s->board[row][i] == num)
            return false;
        if (s->board[i][col] == num)
            return false;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (s->board[startRow + i][startCol + j] == num)
                return false;

    return true;
}

/* ================= SOLVER ================= */

bool solveSudoku(Sudoku *s)
{
    int row = -1, col = -1;
    bool empty = false;

    for (int i = 0; i < SIZE && !empty; i++)
    {
        for (int j = 0; j < SIZE && !empty; j++)
        {
            if (s->board[i][j] == 0)
            {
                row = i;
                col = j;
                empty = true;
            }
        }
    }

    if (!empty)
        return true;

    for (int num = 1; num <= 9; num++)
    {
        if (isSafe(s, row, col, num))
        {
            s->board[row][col] = num;

            if (solveSudoku(s))
                return true;

            s->board[row][col] = 0;
        }
    }

    return false;
}

/* ================= INPUT ================= */

void makeMove(Sudoku *s)
{
    int row, col, num;

    printf("Enter row (0-8): ");
    scanf("%d", &row);

    printf("Enter column (0-8): ");
    scanf("%d", &col);

    printf("Enter number (1-9): ");
    scanf("%d", &num);

    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE)
    {
        printf("Invalid position.\n");
        return;
    }

    if (s->board[row][col] != 0)
    {
        printf("Cell already filled.\n");
        return;
    }

    if (!isSafe(s, row, col, num))
    {
        printf("Move violates Sudoku rules.\n");
        return;
    }

    s->board[row][col] = num;
}

/* ================= CHECK WIN ================= */

bool isComplete(Sudoku *s)
{
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (s->board[i][j] == 0)
                return false;

    return true;
}

/* ================= MAIN GAME LOOP ================= */

int main()
{
    Sudoku game = {0};

    printf("=== SUDOKU GAME ===\n");
    printf("Fill the board manually.\n");
    printf("Commands:\n");
    printf("1 - Make move\n");
    printf("2 - Solve puzzle\n");
    printf("3 - Display board\n");
    printf("4 - Quit\n");

    int choice;

    while (1)
    {
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            makeMove(&game);
            break;

        case 2:
            if (solveSudoku(&game))
            {
                printf("Solved board:\n");
                display(&game);
            }
            else
            {
                printf("No solution exists.\n");
            }
            break;

        case 3:
            display(&game);
            break;

        case 4:
            printf("Goodbye.\n");
            return 0;

        default:
            printf("Invalid choice.\n");
        }

        if (isComplete(&game))
        {
            printf("\n🎉 Puzzle completed!\n");
            display(&game);
            break;
        }
    }

    return 0;
}