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
    FULL,
    ERROR
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
void printBoard(const Board *board);
GStatus playerInput(Player *player, Board *board);
char checkWinner(const Board *board);
BStatus boardFilled(const Board *board);