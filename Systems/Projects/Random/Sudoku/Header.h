#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 9

typedef struct SudokuBoard
{
    int board[SIZE][SIZE];
} Sudoku;

void display(Sudoku *s);
bool isSafe(Sudoku *s, int row, int col, int num);
bool solveSudoku(Sudoku *s);
void makeMove(Sudoku *s);
bool isComplete(Sudoku *s);
