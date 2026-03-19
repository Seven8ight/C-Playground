#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct player
{
    char *letter;
} Player;

typedef struct matrixBoard
{
    char *board;
} Board;

Board *createBoard();
void printBoard(Board *board);
void playerInput(Player *player, Board *board);
char checkWinner(Board *board);
bool boardFilled(Board *board);