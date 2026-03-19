#include "Header.h"

Board *createBoard()
{
    Board *gameBoard = malloc(sizeof(Board));

    if (gameBoard == NULL)
    {
        perror("Not enough memory");
        return NULL;
    }

    char *board = calloc(9, sizeof(char));

    if (!board)
    {
        perror("Not enough memory");
        free(gameBoard);
        return NULL;
    }

    gameBoard->board = board;

    return gameBoard;
}

void printBoard(Board *board)
{
    if (!board)
    {
        perror("Board memory corrupted");
        return;
    }

    int j = 1;

    for (int i = 0; i < 3; i++)
    {
        char letter = (board->board[j - 1] == 'X' || board->board[j - 1] == 'O') ? board->board[j - 1] : (char)95,
             letter2 = (board->board[j] == 'X' || board->board[j] == 'O') ? board->board[j] : (char)95,
             letter3 = (board->board[j + 1] == 'X' || board->board[j + 1] == 'O') ? board->board[j + 1] : (char)95;

        printf("     |     |    \n");
        printf("__%c__|__%c__|__%c__\n", letter, letter2, letter3);
        printf("     |     |    \n");

        j += 3;
    }
}

Status boardFilled(Board *board)
{
    if (!board)
    {
        perror("Board memory corrupted");
        exit(-1);
    }

    for (int i = 0; i < 9; i++)
    {
        if (board->board[i] != 'X' && board->board[i] != 'O')
            return CONTINUE;
    }

    return FULL;
}

void playerInput(Player *player, Board *board)
{
    if (!board || !player)
    {
        perror("Memory corrupted for both player and board");
        return;
    }

    int position;

    printf("Player %c position: ", (char)player->letter);
    scanf("%d", &position);

    if (position <= 0 || position > 9)
        while (position <= 0 || position > 9)
        {
            printf("Try again, range [1-9]: ");
            scanf("%d", &position);
        }

    if (board->board[position - 1] == 'X' || board->board[position - 1] == 'O')
        while (board->board[position - 1] == 'X' || board->board[position - 1] == 'O')
        {
            printf("Position occupied try again: ");
            scanf("%d", &position);
        }

    board->board[position - 1] = (char)player->letter;

    printBoard(board);

    char winner = checkWinner(board);
    if (winner != NO_WINNER)
    {
        printf("Player %c wins", player->letter);
        free(player);
        free(board->board);
        free(board);
        exit(0);
    }

    bool boardCheck = boardFilled(board);
    if (boardCheck == FULL)
    {
        printf("Tie\n");
        free(player);
        free(board->board);
        free(board);
        exit(-1);
    }
}

char checkWinner(Board *board)
{
    if (!board)
    {
        perror("Board memory is corrupted");
        return NO_WINNER;
    }

    if ((board->board[0] == board->board[1] && board->board[1] == board->board[2]) && (board->board[0] == 'X' || board->board[0] == 'O'))
        return board->board[0];
    else if ((board->board[3] == board->board[4] && board->board[4] == board->board[5]) && (board->board[3] == 'X' || board->board[3] == 'O'))
        return board->board[3];
    else if ((board->board[6] == board->board[7] && board->board[7] == board->board[8]) && (board->board[6] == 'X' || board->board[6] == 'O'))
        return board->board[6];
    else if ((board->board[0] == board->board[4] && board->board[4] == board->board[8]) && (board->board[0] == 'X' || board->board[0] == 'O'))
        return board->board[0];
    else if ((board->board[1] == board->board[4] && board->board[4] == board->board[7]) && (board->board[1] == 'X' || board->board[1] == 'O'))
        return board->board[1];
    else if ((board->board[2] == board->board[4] && board->board[4] == board->board[6]) && (board->board[2] == 'X' || board->board[2] == 'O'))
        return board->board[2];
    else if ((board->board[0] == board->board[3] && board->board[3] == board->board[6]) && (board->board[0] == 'X' || board->board[2] == 'O'))
        return board->board[0];
    else if ((board->board[1] == board->board[4] && board->board[4] == board->board[7]) && (board->board[1] == 'X' || board->board[2] == 'O'))
        return board->board[0];
    else if ((board->board[2] == board->board[5] && board->board[5] == board->board[8]) && (board->board[2] == 'X' || board->board[2] == 'O'))
        return board->board[0];
    else
        return NO_WINNER;
}

int main()
{
    Board *board = createBoard();

    char letter, letter2;
    printf("Player 1, Pick a letter, X or O: ");
    scanf("%c", &letter);

    if (toupper(letter) != 'X' && toupper(letter) != 'O')
        while (toupper(letter) != 'X' && toupper(letter) != 'O')
        {
            printf("Player 1, try again, X or O: ");
            scanf(" %c", &letter);
        }

    letter = toupper(letter);

    Player *player1 = malloc(sizeof(Player)),
           *player2 = malloc(sizeof(Player));

    if (letter == 'X')
    {
        letter2 = 'O';
        player1->letter = letter;
        player2->letter = letter2;
    }
    else
    {
        letter2 = 'X';
        player1->letter = letter;
        player2->letter = letter2;
    }

    int playerPlaying = 1;

    while (1)
    {
        if (playerPlaying == 1)
        {
            playerInput(player1, board);
            playerPlaying = 2;
        }
        else
        {
            playerInput(player2, board);
            playerPlaying = 1;
        }
    }

    printf("Ended the game");

    return 0;
}