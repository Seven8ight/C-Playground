#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define NO_WINNER '\0'

typedef enum BoardStatus
{
    CONTINUE,
    FULL
} BStatus;

typedef enum GameStatus
{
    WIN,
    TIE,
    IN_PROGRESS,
    ERROR
} GStatus;

typedef struct player
{
    char letter;
} Player;

typedef struct matrixBoard
{
    char *board;
} Board;

Board *createBoard();
void printBoard(Board *board);
GStatus playerInput(Player *player, Board *board);
char checkWinner(Board *board);
BStatus boardFilled(Board *board);