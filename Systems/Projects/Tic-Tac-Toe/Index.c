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

void printBoard(const Board *board)
{
    if (!board)
    {
        perror("Board memory corrupted");
        return;
    }

    int j = 1;

    for (int i = 0; i < 3; i++)
    {
        char letter = (board->board[j - 1] == 'X' || board->board[j - 1] == 'O') ? board->board[j - 1] : '_',
             letter2 = (board->board[j] == 'X' || board->board[j] == 'O') ? board->board[j] : '_',
             letter3 = (board->board[j + 1] == 'X' || board->board[j + 1] == 'O') ? board->board[j + 1] : '_';

        printf("     |     |    \n");
        printf("__%c__|__%c__|__%c__\n", letter, letter2, letter3);
        printf("     |     |    \n");

        j += 3;
    }
}

BStatus boardFilled(const Board *board)
{
    if (!board)
    {
        perror("Board memory corrupted");
        return ERROR;
    }

    for (int i = 0; i < 9; i++)
    {
        if (board->board[i] != 'X' && board->board[i] != 'O')
            return CONTINUE;
    }

    return FULL;
}

GStatus playerInput(Player *player, Board *board)
{
    if (!board || !player)
    {
        perror("Memory corrupted for both player and board");
        return ERROR;
    }

    int position;

    while (true)
    {
        printf("Player %c position: ", (char)player->letter);

        if (scanf("%d", &position) != 1)
        {
            printf("Invalid input\n");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (position <= 0 || position > 9)
        {
            printf("Invalid input, range [1-9]: ");
            while (getchar() != '\n')
                ;
            continue;
        }

        if (board->board[position - 1] == 'X' || board->board[position - 1] == 'O')
        {
            printf("Position occupied, try again: ");
            while (getchar() != '\n')
                ;
            continue;
        }
        break;
    }

    board->board[position - 1] = (char)player->letter;

    printBoard(board);

    char winner = checkWinner(board);
    if (winner != NO_WINNER)
    {
        printf("Player %c wins", player->letter);
        return WIN;
    }

    bool boardCheck = boardFilled(board);
    if (boardCheck == FULL || boardCheck == ERROR)
        return TIE;

    return IN_PROGRESS;
}

char checkWinner(const Board *board)
{
    if (!board)
    {
        perror("Board memory is corrupted");
        return NO_WINNER;
    }

    int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 4, 8}, {2, 4, 6}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}};

    for (int i = 0; i < 8; i++)
    {
        int position1 = wins[i][0],
            position2 = wins[i][1],
            position3 = wins[i][2];

        if (board->board[position1] && (board->board[position1] == board->board[position2] && board->board[position2] == board->board[position3]))
            return board->board[position1];
    }

    return NO_WINNER;
}

int main()
{
    Board *board = createBoard();

    char letter, letter2;
    printf("Player 1, Pick a letter, X or O: ");
    scanf(" %c", &letter);

    if (toupper(letter) != 'X' && toupper(letter) != 'O')
        while (toupper(letter) != 'X' && toupper(letter) != 'O')
        {
            printf("Player 1, try again, X or O: ");
            scanf(" %c", &letter);
        }

    letter = toupper(letter);

    Player *player1 = malloc(sizeof(Player)),
           *player2 = malloc(sizeof(Player));

    if (!player1 || !player2)
    {
        perror("Not enough memory");

        if (!player1)
            free(player1);
        else
            free(player2);

        free(board->board);
        free(board);
        return -1;
    }

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
    GStatus game = IN_PROGRESS;

    while (game == IN_PROGRESS)
    {
        if (playerPlaying == 1)
        {
            game = playerInput(player1, board);
            playerPlaying = 2;
        }
        else
        {
            game = playerInput(player2, board);
            playerPlaying = 1;
        }
    }

    free(board->board);
    free(board);
    free(player1);
    free(player2);

    printf("Ended the game");

    return 0;
}