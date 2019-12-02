#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum eGameStatus {
    GAME_IN_PROGRESS, ONE_PIECE_REMAINING
}
    GAME_STATUS;

typedef struct nim_game_ai {
    int ai_index;
    int first_row_sticks;
    int second_row_sticks;
    int third_row_sticks;
    int xor_val;
    int winning_move_index;
    int next_best_move_index;
}
	NIM_GAME_AI;

#define MAX_MOVES 192

NIM_GAME_AI nim_game_ai_pool[MAX_MOVES];

void printBoard(int gameBoard[]);
int returnStatus(int cell1, int cell2, int cell3);
void printGameState(GAME_STATUS gameStatus);
GAME_STATUS getStateOfTheGame(int gameBoard[]);
GAME_STATUS checkGameStatus(int gameBoard[]);

int updateBoard (char ** commands, int gameBoard[]);
int convertInputToArrayIndex(int val);

int  playGame(int gameBoard[] );
void computerGame(int gameBoard[] );

int main(int argc, char * argv[] );

int player;
int gameBoard[3];


void printBoard(int gameBoard[3])
{
    int i, j;
  
    for(i = 0; i < 3; i++) {
        printf("Row %d -> ", i+1);
        for (j = 0; j < gameBoard[i]; j++) {
            printf("| ");
        }
        printf("\n");
    }
}



GAME_STATUS checkGameStatus(int gameBoard[3])
{
    int sum = gameBoard[0]+gameBoard[1]+gameBoard[2];
    /* sum = number of matchsticks left */
    return ((sum == 1) ? ONE_PIECE_REMAINING : GAME_IN_PROGRESS); 
}



GAME_STATUS getStateOfTheGame()
{ 
GAME_STATUS gameStatus = checkGameStatus(gameBoard);
    printGameState(gameStatus);
    return gameStatus;
}

void printGameState(GAME_STATUS gameStatus)
{
    if (gameStatus == ONE_PIECE_REMAINING) {
        printf("Current player lost the game\n");
    }
}

int playGame(int gameBoard[] )
{
char line[50];
char * commands[30];
int line_len;
char * split_line;
int index;

    printf("Human player, make your move.\n");
    fflush(stdin);   
    line[0] = 0;
    fgets(line, 10, stdin);
    printf("** You entered %s\n", line);

    line_len = strlen(line);
    line[line_len - 1] = '\0';
    split_line = strtok(line, " ");
    index = 0;
    while (split_line != NULL)
    {
        commands[index] = split_line;
        split_line = strtok(NULL, " ");
        index++;
    }
    commands[index] = NULL;
    if (index != 2) {
        printf("Error, format is involved, command syntax is row_number number_of_sticks_to_take_out\n");
        return 1;
    }
    return (updateBoard(commands, gameBoard));
}

int convertInputToArrayIndex(int val)
{ 
  switch (val)
    {
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 2;
    }
  return -1;
}

int  updateBoard(char ** commands, int gameBoard[3])
{
    char * r = commands[0];
    char * n = commands[1];
    int row = convertInputToArrayIndex(atoi(r));
    if (row == -1) {
       printf("Invalid row number %s\n", commands[0]);
       return 1;
    }
    int cnt = atoi(n);
    if (gameBoard[row] < cnt) {
       printf("Error, row %d only has %d sticks, so %d sticks can not be taken out\n", row+1, gameBoard[row], cnt );
       return 1;
    }
    gameBoard[row]-=cnt;
    printBoard(gameBoard);
    return 0;
}

void computerGame(int gameBoard[] )
{
int i, j, k, res;
static int start_of_search = 0;
int searches_done;

    searches_done = 0;
    for (i = start_of_search; i < MAX_MOVES; i++) {
        if (nim_game_ai_pool[i].first_row_sticks == gameBoard[0]) {
            if (nim_game_ai_pool[i].second_row_sticks == gameBoard[1]) {
                if (nim_game_ai_pool[i].third_row_sticks == gameBoard[2]) {
                    printf("Match found at position %d after %d searches\n", i, searches_done);
                    if (nim_game_ai_pool[i].winning_move_index != -1) {
                        printf("Computer has a winning strategy\n");
                        j = nim_game_ai_pool[i].winning_move_index;
                        start_of_search = j;
                        gameBoard[0] = nim_game_ai_pool[j].first_row_sticks;
                        gameBoard[1] = nim_game_ai_pool[j].second_row_sticks;
                        gameBoard[2] = nim_game_ai_pool[j].third_row_sticks;
                        printBoard(gameBoard);
                        return;
                    }
                    else if (nim_game_ai_pool[i].next_best_move_index != -1) {
                        printf("Computer does not have a winning strategy\n");
                        j = nim_game_ai_pool[i].next_best_move_index;
                        start_of_search = j;
                        gameBoard[0] = nim_game_ai_pool[j].first_row_sticks;
                        gameBoard[1] = nim_game_ai_pool[j].second_row_sticks;
                        gameBoard[2] = nim_game_ai_pool[j].third_row_sticks;
                        printBoard(gameBoard);
                        return;
                    }
                    else {
                        printf("Computer does not have any move for this situation.\n");
                        return;
                    }
                }
            }
        }
        searches_done++;
    } 
    printf("Did not find any matching strategy, so no move\n");
    return;   
}

int main(int argc, char * argv[] )
{
int gameStatus = GAME_IN_PROGRESS;
char c;
int i, j;
int seq;
int res;
FILE * fNim;
int ai_index;
int ai_first;
int ai_second;
int ai_third;
int xor_val;
int winning_boolean;
int winning_index;
int next_move_index;
  
    if (argc!=2) {
        printf("Error, invalid parameters\n");
        printf("Usage - nim_ai 1 to play as player 1\n");
        printf("Or nim_ai 2 to play as player 2\n");
        return 1;
    }
    c = *argv[1];
    if ((c != '1') && (c != '2')) {
        printf("Error - must specify 1 or 2\n");
        return 1;
    }
    printf("\nWelcome to the wonderful game of NIM\nInitializing game of NIM\n");
    gameBoard[0] = 7;
    gameBoard[1] = 5;
    gameBoard[2] = 3;
    printBoard(gameBoard);
    fNim = fopen("nim_data.csv", "rt");
    if (fNim == NULL) {
        printf("Error, nim AI file nim_data.csv not found\n");
        return 1;
    }
    while (!feof(fNim)) {
        ai_index = -1;
        ai_first = -1;
        ai_second = -1;
        ai_third = -1;
        xor_val = -1;
        winning_boolean = -1;
        winning_index = -1;
        int next_move_index = -1;
        fscanf(fNim,"%d,%d,%d,%d,%d,%d,%d,%d\n", &ai_index, &ai_first, &ai_second, &ai_third, &xor_val, &winning_boolean, &winning_index, &next_move_index);
        //printf("%d,%d,%d,%d,%d,%d,%d,%d\n", ai_index, ai_first, ai_second, ai_third, xor_val, winning_boolean, winning_index, next_move_index);
        if (ai_index == -1) {
            printf("Error, table index %d appears to be corrupted\n", ai_index);
            fclose(fNim);
            return 1;
        }
        if (((winning_boolean == 1) && (winning_index == -1))||((winning_boolean == 0) && (winning_index != -1))) {
            printf("Error, table index %d appears to be corrupted\n", ai_index );
            fclose(fNim);
            return 1;
        }
        nim_game_ai_pool[ai_index].ai_index = ai_index;
        nim_game_ai_pool[ai_index].first_row_sticks = ai_first;
        nim_game_ai_pool[ai_index].second_row_sticks = ai_second;
        nim_game_ai_pool[ai_index].third_row_sticks = ai_third;
        nim_game_ai_pool[ai_index].xor_val = xor_val;
        nim_game_ai_pool[ai_index].winning_move_index = winning_index;
        nim_game_ai_pool[ai_index].next_best_move_index = next_move_index;

    }
    fclose(fNim);
    for (i = 1; i < MAX_MOVES; i++) {
        if (nim_game_ai_pool[i].winning_move_index != -1) {
            j = nim_game_ai_pool[i].winning_move_index;
            if (nim_game_ai_pool[j].winning_move_index != -1) {
                printf("Error, table index %d appears to be corrupted\n", i);
                return 1;
            }
        }
    }
    printf("Instructions: Input your command in the following structure: Row# Number_of_sticks_to_take_out#\n");
    printf("For example, to take out 3 sticks from row 1, which originally holds 7 sticks, you would enter: 1 3\n");
    printf("To take out 2 sticks from row 2, which originally holds 5 sticks, you would enter: 2 2\n");
    printf("To take out 1 stick from row 3, which originally holds 3 sticks, you would enter: 3 1\n");
    printf("Whoever takes out the last stick loses the game\n");
    printf("Enjoy!!\n");
    fflush(stdin);
    printf("You elected to play as player %c\n", c);
       
    while (gameStatus == GAME_IN_PROGRESS)
    {
        if (c  == '1') {
            while (playGame(gameBoard))
                ;
            gameStatus = getStateOfTheGame(gameBoard);
            if (gameStatus != GAME_IN_PROGRESS)
                break;
            computerGame(gameBoard);
            gameStatus = getStateOfTheGame(gameBoard);
            if (gameStatus != GAME_IN_PROGRESS)
                break;
        }
        else {
            computerGame(gameBoard);
            //printBoard(gameBoard);
            gameStatus = getStateOfTheGame(gameBoard);
            if (gameStatus != GAME_IN_PROGRESS)
                break;
            while (playGame(gameBoard))
                ;
            gameStatus = getStateOfTheGame(gameBoard);
            if (gameStatus != GAME_IN_PROGRESS)
                break;
        }
    }
}

