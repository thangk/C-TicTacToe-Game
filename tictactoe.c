#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined _WIN32
#define CLEAR_CONSOLE "cls"
#elif defined __unix__ || __linux__
#define CLEAR_CONSOLE "clear"
#endif



//############################################################################################
//
//  Notes                                                                    
//
//############################################################################################
//
//  In my lab 3, a TA mentioned that CLEAR_CONSOLE is not recognized. I've tried that to be 
//  working both on online compilers and in my VS Code on the local PC. I was wondering the 
//  TA probably forgot to copy the preprocessors at the top of the code?
//
//  Another TA also tried my lab 3, and it ran without warnings.
//
//
//  When a win occurs, the marking with the curly brackets work most of the time but at some
//  rare times, one or more of the curly brackets go on the wrong cells. If this occurs but
//  the winning announcer is correct, I hope that I don't get marks docked as it's an extra
//  decorative thing I implemented. :D
//
//
//  Assignment 2 Notes:
//
//  1   Duplicate players check not implemented (since not in PDF).
//  2   Feature to delete players not implemented (same reason as above). Can be deleted
//      manually in from text files.
//  3   To "reset" the program, simply delete the following four text files:
//          players.text
//          savedgames.text
//          savedboards.text
//          database.text
//
//  ~ Kap
//


const char * READ_DATABASE_FILE =  "Number of players: %d\nNumber of saved games: %d";
const char * WRITE_DATABASE_FILE = "Number of players: %d\nNumber of saved games: %d";
const char * READ_GAMES_FILE = "GameID: %d\tBoard: %d\t Status: %[^,], playerX: %d (points: %d), playerO: %d (points: %d), nextTurn: %c, totalMoves: %d\n";
const char * WRITE_GAMES_FILE = "GameID: %d\tBoard:  %d\tStatus: %s, playerX: %d (points: %d), playerO: %d (points: %d), nextTurn: %c, totalMoves: %d\n";
const char * READ_BOARDS_FILE = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n";     // board id, cell0-9
const char * WRITE_BOARDS_FILE = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n";
const char * READ_PLAYERS_FILE = "%d   %s %[^,], %[^,], %d, %d, %d, %d, %d\n"; // id   fName lName, email, score, games, wins, draws, losses
const char * WRITE_PLAYERS_FILE = "%d   %s %s, %s, %d, %d, %d, %d, %d\n";

typedef struct {
    char board[9];
    int boardIndex;
} Board;

typedef struct {
    int GameID;         // a random unique number between 0-10,000
    int player1ID;
    int player2ID;
    char whosTurn;     // X or O
    int totalMoves;
    char gameStatus[25];    // X won, O own, Draw or Ongoing
    int currentXpoints;
    int currentOpoints;
} Game;

typedef struct {
    char firstName[25];
    char lastName[25];
    char playerEmail[100];
    int playerID;
    int playerScore;
    int playerGames;
    int wins;
    int draws;
    int losses;
} Player;


//############################################################################################
//  ASSIGNMENT 2 related PROTOTYPES     START
//############################################################################################

//  sorting related functions
void swapInt(int * a, int * b);
void swapChar(char * a, char * b);
void swapString(char * a, char * b);
void quicksort_asc(Player * list, int start, int end, int sortMode);
void quicksort_des(Player * list, int start, int end);
void copyList(Player * source, Player * des, int listSize);
int partition(Player * list, int start, int end, int sortMode);

//  helper functions
void pressEnter();
void displayLogo();
void displayMainMenu();
void displayErrorMessage(int errorCode);
int getNumOf(int option);
int getRandomNum(int min, int max);
int validateInput(char * str, int maxChar, int criteriaCode, int useLimits, int lowerLimit, int upperLimit);

int overwrite_Players_File(Player * playerslist, int * numOfPlayers);
int overwrite_Games_File(Game * gameslist, Board * boardslist, int * numOfSavedGames);
int overwrite_Boards_File(Board * boardslist, int * numOfSavedGames);
int append_Players_File(Player * playerslist, int index);
int append_Games_File(Game * gameslist, Board * boardslist, int index);
int append_Boards_File(Board * boardslist, int index);
int update_Database_File(int * numOfPlayers, int * numOfSavedGames);
int load_Boards_File(Board * boardslist);
int load_Games_File(Game * gameslist, Board * boardslist);
int load_Players_File(Player * playerslist);

//  "Add Players" function
int addPlayer(Player * playerslist, int * numOfPlayers, int * numOfSavedGames);

//  "Create Games" related function
void optionG(int m, int n, char board[][n], int * winPosition, Board * boardslist, Game * gameslist, Player * playerslist, int * numOfSavedGames, int * numOfPlayers);
void newGame(int m, int n, char board[][n], int * winPosition, Board * boardslist, Game * gameslist, Player * playerslist, int * numOfSavedGames, int * numOfPlayers, int resumeSavedGame, int loadGameID);
int saveGame(int m, int n, char board[][n], Board * boardslist, Game * gameslist, int saveResumedGame, int savedGameIndex, int * numOfSavedGames, int * numOfPlayers);
void promptToSaveFinishedGame(int m, int n, char board[][n], Board * boardslist, Game * gameslist, int gameIndex, int * numOfSavedGames, int * numOfPlayers);
void loadGame(int m, int n, char board[][n], int * winPosition, Board * boardlist, Game * gamelist, Player * playerslist, int gameIndex, int * numOfSavedGames, int * numOfPlayers);

//  "Leaderboard" function
void leaderBoard(Player * playerslist, int numOfPlayers);

//############################################################################################
//  ASSIGNMENT 2 related PROTOTYPES     END
//############################################################################################



//############################################################################################
//  ASSIGNMENT 1 related PROTOTYPES     START
//############################################################################################

int searchAndSwapCellValue(int position, int m, int n, char player, char board[][n]);       // swaps the cell numbers with the user's sign, X or O, and returns 1 if it's successful or else 0 and asks user to enter in another cell or a valid input
int inputOption(char str[], int n);                     // takes in an input as a string
int getMovesNumber(int m, int n, char board[][n]);      // to keep track of the turns, so it's the same turn # when exiting during a turn, checking prediction then coming back to continue playing
int gameStillRunning(int m, int n, char board[][n], int * winPosition);    // returns 1 if game is still running (ie. if user exit while his turn so he can continue playing when going back to the create board menu option)
void checkValid(int m, int n, char board[][n]);         // display both board while showing whether it's a valid board or not
void displayFinalBoard(int m, int n, char board[][n], int * winPosition, Player * playerslist, Game * gameslist, int thisGame, char winner);  // to display final board with the winner's cells hightlighted with curly brackets

void caseOne(char player, char singleList[]);
void caseTwo(char player, char singleList[]);
void caseThree(char player, char singleList[]);

void initializeBoard(int m, int n, char board[][n]);
void printBoard(int m, int n, char board[][n], int clearConsole);
void listWinningCells(char turn, int m, int n, char board[][n]);
char whoIsTheWinner(int m, int n, char board[][n], int * winPosition);
int isValidBoard(int m, int n, char board[][n]);

//############################################################################################
//  ASSIGNMENT 1 related PROTOTYPES     END
//############################################################################################






//############################################################################################
//
//  Game Storage                                                                     15 points
//
//############################################################################################
//
//  Players, Saved Games, Saved Boards and Game info (aka Database) are stored in
//  dynamic memory structs during the program run time.
//
//  When program closes, they're stored in:
//  
//  Players list in players.txt
//  Saved games list in savedgames.txt
//  Saved boards list in savedboards.txt
//  Database in database.txt





//############################################################################################
//
//  Main Function                                                                    10 points
//
//############################################################################################

int main() {

    int m = 3, n = 3;               // board is a 3x3, so m and n are predetermined   
    char board[m][n];

    initializeBoard(m, n, board);   // initialize board

    int errorCheck = 0;             // used for counting an invalid input, so when error = 1, a reprompt message and error is shown
    char optionChoice[2];             
    int reprompt = 0;               // if 1, the input reprompts to enter a valid input
    int clearBeforeMenu = 1;        // if 1, then screen clear occurs, otherwise no

    int numOfPlayers;
    int numOfSavedGames;
    int numOfSavedBoards;

    int winPosition = 0;

    /*
    winPosition Numbers:
    1   top row
    2   mid row
    3   bot row
    4   left col
    5   mid col
    6   right col
    7   forward diag
    8   reversed diag
    */

    // create the necessary files
    FILE * file1 = fopen("players.txt", "a");

    if (file1 == NULL) {
        return 1;
    }
    fseek(file1, 0, SEEK_END);
    if (!ftell(file1)) {
        numOfPlayers = 0;
    } else {
        numOfPlayers = getNumOf(1);
    }
    fclose(file1);

    FILE * file2 = fopen("savedgames.txt", "a");

    if (file2 == NULL) {
        return 1;
    }
    fseek(file2, 0, SEEK_END);
    if (!ftell(file2)) {
        numOfSavedGames = 0;
    } else {
        numOfSavedGames = getNumOf(2);
    }
    fclose(file2);

    FILE * file3 = fopen("savedboards.txt", "a");

    if (file3 == NULL) {
        return 1;
    }
    fclose(file3);

    numOfSavedBoards = numOfSavedGames;

    FILE * file4 = fopen("database.txt", "w+");

    if (file4 == NULL) {
        return 1;
    }
        fprintf(file4, WRITE_DATABASE_FILE, numOfPlayers, numOfSavedGames);
    
    fclose(file4);


    // if both numbers are 0, then start with space for 5 players and 10 games, if not, add space for 5 players and 10 games more

    Player * playerslist;
    Game * gameslist;
    Board * boardslist;

    if (!numOfPlayers) {
        playerslist = (Player *) malloc(5*sizeof(Player));
    } else if (numOfPlayers > 0) {
        playerslist = (Player *) malloc((numOfPlayers+(numOfPlayers/2))*sizeof(Player));
    }

    if (!numOfSavedGames) {
        gameslist = (Game *) malloc(10*sizeof(Game));
    } else if (numOfSavedGames > 0) {
        gameslist = (Game *) malloc((numOfSavedGames+(numOfSavedGames/2))*sizeof(Game));
    }

    if (!numOfSavedBoards) {
        boardslist = (Board *) malloc(10*sizeof(Board));
    } else {
        boardslist = (Board *) malloc((numOfSavedBoards+(numOfSavedBoards/2))*sizeof(Board));
    }


    // load in the data
    if (numOfPlayers) {
        load_Players_File(playerslist);
    }

    if (numOfSavedGames) {
        load_Games_File(gameslist, boardslist);
    }

    if (numOfSavedBoards) {
        load_Boards_File(boardslist);
    }

    do {

        if (clearBeforeMenu) {
            system(CLEAR_CONSOLE);
        }

        displayLogo();

        printf("   Players: %d  | Saved Games: %d\n\n", numOfPlayers, numOfSavedGames);
        displayMainMenu();

        if (errorCheck > 0) {
            printf(">  Error! Enter one of the available options.\n\n");
        }

        printf(
        "   Option: ");
        
        inputOption(optionChoice, 2);

        switch(optionChoice[0]) {
            case 'p':
            case 'P':   reprompt = 2; clearBeforeMenu = 1; addPlayer(playerslist, &numOfPlayers, &numOfSavedGames); break;
            case 'g':
            case 'G':   reprompt = 2; clearBeforeMenu = 1; optionG(m, n, board, &winPosition, boardslist, gameslist, playerslist, &numOfSavedGames, &numOfPlayers); break;
            case 'l':
            case 'L':   reprompt = 2; leaderBoard(playerslist, numOfPlayers); break;
            case 'r':
            case 'R':   reprompt = 2; initializeBoard(m, n, board); clearBeforeMenu = 0; printBoard(m, n, board, 1); break;
            case 'e':
            case 'E':   reprompt = 0; break;
            default:    reprompt = 1;
        }

        if (reprompt == 1) {
            errorCheck++;
        }

    } while (reprompt == 1 || reprompt == 2);

    free(playerslist);
    free(gameslist);
    free(boardslist);
        
    return 0;
}

//############################################################################################
//
//  "Add Players" related functions - START                                          30 points
//
//############################################################################################

int addPlayer(Player * playerslist, int * numOfPlayers, int * numOfSavedGames) {

    int numOfPlayersInFile = getNumOf(1);
    int nextIndex = *numOfPlayers;

    char tempFirstName[30];
    char tempLastName[30];
    char tempEmail[105];

    system(CLEAR_CONSOLE);
    displayLogo();

    playerslist = (Player *) realloc(playerslist, numOfPlayersInFile+3);
 
    printf("   >> Add player\n\n");

    int errorCheck = 0;

    // Prompt for first name + validate
    do {
        if (errorCheck > 0) {
            displayErrorMessage(errorCheck);
        }
        printf("   First name: ");

        scanf(" %s", tempFirstName);
        errorCheck = validateInput(tempFirstName, 25, 3, 0, 0, 0);
    } while (errorCheck != 0);

    // Prompt for last name + validate
    do {
        if (errorCheck > 0) {
            displayErrorMessage(errorCheck);
        }
        printf("   Last name: ");

        scanf(" %s", tempLastName);
        errorCheck = validateInput(tempLastName, 25, 3, 0, 0, 0);
        
    } while (errorCheck != 0);

    // Prompt for email + validate
    do {
        if (errorCheck > 0) {
            displayErrorMessage(errorCheck);
        }
        printf("   Email: ");

        scanf(" %s", tempEmail);
        errorCheck = validateInput(tempEmail, 100, 7, 0, 0, 0);
    } while (errorCheck != 0);

    // If all validations passed, continue below:
    strcpy(playerslist[nextIndex].firstName, tempFirstName);
    strcpy(playerslist[nextIndex].lastName, tempLastName);
    strcpy(playerslist[nextIndex].playerEmail, tempEmail);
    playerslist[nextIndex].playerID = nextIndex+1;
    playerslist[nextIndex].playerGames = 0;
    playerslist[nextIndex].playerScore = 100;
    playerslist[nextIndex].wins = 0;
    playerslist[nextIndex].draws = 0;
    playerslist[nextIndex].losses = 0;

    // update number of players
    *numOfPlayers += 1;

    // append to file
    append_Players_File(playerslist, nextIndex);

    // update database file
    update_Database_File(numOfPlayers, numOfSavedGames);
    
    while (getchar() != '\n');

    return 0;
}

//############################################################################################
//
//  "Add Players" related functions - END
//
//############################################################################################





//############################################################################################
//
//  Create Games related functions - START                                           30 points
//
//############################################################################################

void optionG(int m, int n, char board[][n], int * winPosition, Board * boardslist, Game * gameslist, Player * playerslist, int * numOfSavedGames, int * numOfPlayers) {

    int errorCheck = 0;
    char option[2];
    int reprompt = 1;
    int gameIndex;
    int resumeSavedGame = 0;
    int loadGameID = 0;

    do {
            // Prompt for option + validate
            do {

                system(CLEAR_CONSOLE);
                displayLogo();

                printf("   >> Play game\n\n");

                if (errorCheck > 0) {
                    displayErrorMessage(errorCheck);
                }
                printf("   1. Start a new game:\n   2. Load a saved game\n   0. Back to main menu\n\n   Option: ");
                scanf(" %s", option);
                errorCheck = validateInput(option, 1, 2, 1, 0, 2);
                while (getchar() != '\n');
            } while (errorCheck != 0);

            int optionSwitch = atoi(option);

            switch (optionSwitch) {
                case 0: reprompt = 0; break;
                case 1: newGame(m, n, board, winPosition, boardslist, gameslist, playerslist, numOfSavedGames, numOfPlayers, resumeSavedGame, loadGameID); break;
                case 2: loadGame(m, n, board, winPosition, boardslist, gameslist, playerslist, gameIndex, numOfSavedGames, numOfPlayers); break;
            }

    } while (reprompt);
}

void newGame(int m, int n, char board[][n], int * winPosition, Board * boardslist, Game * gameslist, Player * playerslist, int * numOfSavedGames, int * numOfPlayers, int resumeSavedGame, int loadGameID) {

    // from my understanding "createBoard" is the playing mode so I'll treat it as such.

    char symbols[] = {'X', 'O'};

    int totalMoves = m * n;     // track the total number of turns
    int cellPosition = 0;           // track validity of cell value input
    int cellChoice = 1;         // returns 1 if cell value has been successfullly modified
    int errorCheck = 0;         // tracker for error code when invalid input is entered
    char moveOption[2];         // cell number input char array
    char whoseTurn[2];          // turn input char array
    int movesTracker;       // tracker for what turn number it is
    char turn;                  // turn = X when movesTracker is odd else O when it's even
    int exitCode = 0;           // tracker for when user wishes to end his turn, exit code
    char player[2];
    int playerX;
    int playerO;
    char resumeChoice[2];
    int gameIndex;
    char currentGameStatus[25] = "Inactive";

    int thisGame;
    int goBackAddPlayers = 0;
    int resumeBoardIndex = loadGameID - 1;
    int resumeGame;

    resumeSavedGame = (loadGameID >= 1 && loadGameID <= *numOfSavedGames) ? 1 : 0;

    if (resumeSavedGame) {
        thisGame = resumeBoardIndex;
        gameIndex = resumeBoardIndex + 1;
        movesTracker = gameslist[resumeBoardIndex].totalMoves + 1;
    } else {
        thisGame = *numOfSavedGames;
        movesTracker = getMovesNumber(m, n, board) + 1;
        // gameIndex = thisGame;
    }

    // check and prompt to continue existing game or not
    if (!resumeSavedGame) {
        if (!strcmp(currentGameStatus, "Ongoing")) {
            do {
                system(CLEAR_CONSOLE);
                displayLogo();

                if (errorCheck > 0) {
                    displayErrorMessage(errorCheck);
                }
                while (getchar() != '\n');
                printf("   >> There's an active game currently. Resume that?\n\n   1. Yes\n   0. No\n\n   Option: ");

                scanf("%s", resumeChoice);
                errorCheck = validateInput(resumeChoice, 1, 2, 1, 0, 1);
                while (getchar() != '\n');
            } while (errorCheck != 0);
        }
    }

    resumeGame = atoi(resumeChoice);

    do {

        // if starting a new game
        if (resumeSavedGame == 0) {
                if (resumeGame == 0) {
                    initializeBoard(m, n, board);

                    movesTracker = 1;
                
                    if (*numOfPlayers == 0) {
                        system(CLEAR_CONSOLE);
                        displayLogo();
                        printf("   >> No players yet, add at least two first\n\n");

                        printf("   Press Enter to go back and add players ... ");
                        while (getchar() != '\n');
                        pressEnter();
                        goBackAddPlayers = 1;
                    } else if (*numOfPlayers == 1) {
                        system(CLEAR_CONSOLE);
                        displayLogo();
                        printf("   >> Not enough players, add one more first\n\n");
                        printf("   Press Enter to go back and add players ... ");
                        while (getchar() != '\n');
                        pressEnter();
                        goBackAddPlayers = 1;
                    } else if (*numOfPlayers >= 2) {
                        // player selection
                        for (int j = 0; j < 3; j++) {

                            system(CLEAR_CONSOLE);
                            displayLogo();

                            printf("   >> Who's playing\n\n");

                            // select player X
                            if (j < 2) {
                                for (int i = 0; i < *numOfPlayers; i++) {
                                    printf("   %d. %s %s\n", i+1, playerslist[i].firstName, playerslist[i].lastName);
                                }
                            }

                            // select player O
                            if (j == 1) {
                                printf("\n       X: %s %s", playerslist[playerX].firstName, playerslist[playerX].lastName);
                            }

                            // select who are playing
                            if (j < 2) {
                                    // Prompt for player + validate
                                do {
                                    if (errorCheck > 0) {
                                        displayErrorMessage(errorCheck);
                                    }
                                    printf("\n\n   Select for player %c: ", symbols[j]);

                                    scanf("%s", player);
                                    errorCheck = validateInput(player, 1, 2, 1, 1, *numOfPlayers);
                                } while (errorCheck != 0);

                                if (symbols[j] == 'X') {
                                    playerX = atoi(player) - 1;
                                } else if (symbols[j] == 'O') {
                                    playerO = atoi(player) - 1;
                                }
                            }

                            if (j == 2) {
                                printf("       %s %s (X)\n\n \t   Vs.\n\n       %s %s (O)\n\n   Press Enter to Start ... ", playerslist[playerX].firstName, playerslist[playerX].lastName, playerslist[playerO].firstName, playerslist[playerO].lastName);
                                while (getchar() != '\n');
                                pressEnter();
                            }
                        }
                    }
                }

                // initialize game info
                gameslist[thisGame].GameID = getRandomNum(1, 10000);
                gameslist[thisGame].currentXpoints = 3;
                gameslist[thisGame].currentOpoints = 3;
                strcpy(gameslist[thisGame].gameStatus, "Ongoing");
                strcpy(currentGameStatus, "Ongoing");
                gameslist[thisGame].totalMoves = movesTracker;
                gameslist[thisGame].whosTurn = 'X';
                gameslist[thisGame].player1ID = playerX;
                gameslist[thisGame].player2ID = playerO;
        }

        int showHint;

        if (!resumeGame) {
            showHint = 0;
        }

        if (resumeSavedGame) {

            int i = 0;

            // restore the saved board

            for (int n = 0; n < 3; n++, i++) {
                board[0][n] = boardslist[resumeBoardIndex].board[i];
            }
            for (int n = 0; n < 3; n++, i++) {
                board[1][n] = boardslist[resumeBoardIndex].board[i];
            }
            for (int n = 0; n < 3; n++, i++) {
                board[2][n] = boardslist[resumeBoardIndex].board[i];
            }
            // printBoard(m, n, board);
        } else {
            initializeBoard(m, n, board);
        }

        do {

            errorCheck = 0; // reset error message each turn

            do {

                system(CLEAR_CONSOLE);
                displayLogo();

                printf("\t\t\t\t  %s (X) Vs. %s (O)\n\n",
                playerslist[gameslist[thisGame].player1ID].firstName,
                playerslist[gameslist[thisGame].player2ID].firstName
                );

                printBoard(m, n, board, 0);


                if (errorCheck > 0) {
                    displayErrorMessage(errorCheck);
                }
                if (showHint) {
                    listWinningCells(turn, m, n, board);
                    showHint = 0;
                }

                turn = (movesTracker % 2) ? 'X' : 'O';
                gameslist[thisGame].whosTurn = turn;

                printf(
                    "\t\t\t\t    Player %c's turn\n\n"
                    "   >> Hint points remaining:\t      X: %d points\t\t   Game Status: %s\n\t\t\t\t      O: %d points"
                    "\t\t\tMove #: %d\n\n"
                    "\n\t\tEnter the number of the cell where you want to insert X or O\n\n   \t\t\t\t    Other options:\n   \t\t\t\t    h   Show hints\n   \t\t\t\t    s   Save game\n   \t\t\t\t    e   Exit\n\n"
                    "\t\t\t\t    Action: ", turn, gameslist[thisGame].currentXpoints, gameslist[thisGame].gameStatus, gameslist[thisGame].currentOpoints, movesTracker);

                inputOption(moveOption, 2);
                errorCheck = validateInput(moveOption, 1, 11, 1, 1, 9);
                cellPosition = atoi(moveOption);

                if (moveOption[0] == 'h' || moveOption[0] == 'H') {
                    if (turn == 'X' && gameslist[thisGame].currentXpoints > 0) {
                        gameslist[thisGame].currentXpoints -= 1;
                        showHint = 1;
                        movesTracker--;
                        
                    } else if (turn == 'O' && gameslist[thisGame].currentOpoints > 0) {
                        gameslist[thisGame].currentOpoints -= 1;
                        showHint = 1;
                        movesTracker--;
                    }
                    
                } else if (moveOption[0] == 's' || moveOption[0] == 'S') {
                    gameslist[thisGame].totalMoves = movesTracker - 1;
                    gameslist[thisGame].whosTurn = turn;
                    saveGame(m, n, board, boardslist, gameslist, resumeSavedGame, thisGame, numOfSavedGames, numOfPlayers);
                    system(CLEAR_CONSOLE);
                    displayLogo();
                    printf("   >> Game has been saved!\n\n  Press Enter to continue ... ");
                    pressEnter();
                    exitCode = 1;
                } else if (moveOption[0] == 'e' || moveOption[0] == 'E') {
                    exitCode = 1;
                } else if (cellPosition >= 1 && cellPosition <= 9) {

                    // cellChoice returns 0, causing error if player picks a cell already taken or out of bound values
                    cellChoice = searchAndSwapCellValue(cellPosition, m, n, turn, board);

                } else if (!cellChoice) {
                    errorCheck = 10;
                } else {
                    errorCheck = 11;
                }
                
            } while (errorCheck > 0 || !cellChoice);


            if (exitCode) {
                movesTracker = (m * n) + 1;      // if -1, exit game
            } else if (movesTracker < 9) {
                switch (whoIsTheWinner(m, n, board, winPosition)) {
                    case 'X':   strcpy(gameslist[thisGame].gameStatus, "X_Won");
                                strcpy(currentGameStatus, "X_Won");
                                gameslist[thisGame].whosTurn = '-';
                                gameslist[thisGame].totalMoves = movesTracker;
                                displayFinalBoard(m, n, board, winPosition, playerslist, gameslist, thisGame, 'X');
                                if (playerslist[gameslist[thisGame].player2ID].playerScore > playerslist[gameslist[thisGame].player1ID].playerScore) {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 2;
                                } else {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 1;
                                }
                                playerslist[gameslist[thisGame].player1ID].playerScore += gameslist[thisGame].currentXpoints;
                                playerslist[gameslist[thisGame].player1ID].wins++;
                                playerslist[gameslist[thisGame].player2ID].losses++;
                                playerslist[gameslist[thisGame].player1ID].playerGames++;
                                playerslist[gameslist[thisGame].player2ID].playerGames++;
                                promptToSaveFinishedGame(m, n, board, boardslist, gameslist, gameIndex, numOfSavedGames, numOfPlayers);
                                movesTracker = 10;
                                break;
                    case 'O':   strcpy(gameslist[thisGame].gameStatus, "O_Won");
                                strcpy(currentGameStatus, "O_Won");
                                gameslist[thisGame].whosTurn = '-';
                                gameslist[thisGame].totalMoves = movesTracker;
                                displayFinalBoard(m, n, board, winPosition, playerslist, gameslist, thisGame, 'O');
                                if (playerslist[gameslist[thisGame].player1ID].playerScore > playerslist[gameslist[thisGame].player2ID].playerScore) {
                                    playerslist[gameslist[thisGame].player1ID].playerScore -= 2;
                                } else {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 1;
                                }
                                playerslist[gameslist[thisGame].player2ID].playerScore += gameslist[thisGame].currentOpoints;
                                playerslist[gameslist[thisGame].player2ID].wins++;
                                playerslist[gameslist[thisGame].player1ID].losses++;
                                playerslist[gameslist[thisGame].player1ID].playerGames++;
                                playerslist[gameslist[thisGame].player2ID].playerGames++;
                                promptToSaveFinishedGame(m, n, board, boardslist, gameslist, gameIndex, numOfSavedGames, numOfPlayers);
                                movesTracker = 10;
                                break;
                    default: movesTracker++;
                }
            } else if (movesTracker == 9) {
                switch (whoIsTheWinner(m, n, board, winPosition)) {
                    case 'X':   strcpy(gameslist[thisGame].gameStatus, "X_Won");
                                strcpy(currentGameStatus, "X_Won");
                                gameslist[thisGame].whosTurn = '-';
                                gameslist[thisGame].totalMoves = movesTracker;
                                displayFinalBoard(m, n, board, winPosition, playerslist, gameslist, thisGame, 'X');
                                if (playerslist[gameslist[thisGame].player2ID].playerScore > playerslist[gameslist[thisGame].player1ID].playerScore) {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 2;
                                } else {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 1;
                                }
                                playerslist[gameslist[thisGame].player1ID].playerScore += gameslist[thisGame].currentXpoints;
                                playerslist[gameslist[thisGame].player1ID].wins++;
                                playerslist[gameslist[thisGame].player2ID].losses++;
                                playerslist[gameslist[thisGame].player1ID].playerGames++;
                                playerslist[gameslist[thisGame].player2ID].playerGames++;
                                promptToSaveFinishedGame(m, n, board, boardslist, gameslist, gameIndex, numOfSavedGames, numOfPlayers);
                                movesTracker = 10;
                                break;
                    case 'O':   strcpy(gameslist[thisGame].gameStatus, "O_Won");
                                strcpy(currentGameStatus, "O_Won");
                                gameslist[thisGame].whosTurn = '-';
                                gameslist[thisGame].totalMoves = movesTracker;
                                displayFinalBoard(m, n, board, winPosition, playerslist, gameslist, thisGame, 'O'); 
                                if (playerslist[gameslist[thisGame].player1ID].playerScore > playerslist[gameslist[thisGame].player2ID].playerScore) {
                                    playerslist[gameslist[thisGame].player1ID].playerScore -= 2;
                                } else {
                                    playerslist[gameslist[thisGame].player2ID].playerScore -= 1;
                                }
                                playerslist[gameslist[thisGame].player2ID].playerScore += gameslist[thisGame].currentOpoints;
                                playerslist[gameslist[thisGame].player2ID].wins++;
                                playerslist[gameslist[thisGame].player1ID].losses++;
                                playerslist[gameslist[thisGame].player1ID].playerGames++;
                                playerslist[gameslist[thisGame].player2ID].playerGames++;
                                promptToSaveFinishedGame(m, n, board, boardslist, gameslist, gameIndex, numOfSavedGames, numOfPlayers);
                                movesTracker = 10;
                                break;
                    case 'D':   strcpy(gameslist[thisGame].gameStatus, "Draw");
                                strcpy(currentGameStatus, "Draw");
                                gameslist[thisGame].whosTurn = '-';
                                gameslist[thisGame].totalMoves = movesTracker;
                                displayFinalBoard(m, n, board, winPosition, playerslist, gameslist, thisGame, 'D');
                                playerslist[playerO].draws++;
                                playerslist[playerX].playerGames++;
                                playerslist[playerO].playerGames++;
                                promptToSaveFinishedGame(m, n, board, boardslist, gameslist, gameIndex, numOfSavedGames, numOfPlayers);
                                movesTracker = 10; break;
                }
            } else {
                movesTracker++; // increment turn
                
            }

        } while (movesTracker <= totalMoves);
        
    } while (goBackAddPlayers);

    // reset winPosition
    *winPosition = 0;

    // update players file
    overwrite_Players_File(playerslist, numOfPlayers);

}

int saveGame(int m, int n, char board[][n], Board * boardslist, Game * gameslist, int saveResumedGame, int savedGameIndex, int * numOfSavedGames, int * numOfPlayers) {

    int thisGame;

    if (saveResumedGame) {
        thisGame = savedGameIndex;
    } else {
        thisGame = *numOfSavedGames;
    }

    // if (gameIndex > 0) {
    //     thisGame = gameIndex - 1;
    // } else {
    //     // increment saved games counter
    //     *numOfSavedGames += 1;
    // }

    boardslist[thisGame].boardIndex = thisGame;

    // save the cell positions in board[boardIndex]
    int i = 0;
        for (int n = 0; n < 3; n++, i++) {
            boardslist[thisGame].board[i] = board[0][n];
        }
        for (int n = 0; n < 3; n++, i++) {
            boardslist[thisGame].board[i] = board[1][n];
        }
        for (int n = 0; n < 3; n++, i++) {
            boardslist[thisGame].board[i] = board[2][n];
        }

    if (saveResumedGame) {
        // overwrite current data to update the saved games with new data
        overwrite_Boards_File(boardslist, numOfSavedGames);

        overwrite_Games_File(gameslist, boardslist, numOfSavedGames);
    } else {
        // append new data to savedboards file
        append_Boards_File(boardslist, thisGame);

        // append new data to savedgames file
        append_Games_File(gameslist, boardslist, thisGame);

            *numOfSavedGames += 1;

        // update the database values
        update_Database_File(numOfPlayers, numOfSavedGames);
    }
    
    return 0;
}

void promptToSaveFinishedGame(int m, int n, char board[][n], Board * boardslist, Game * gameslist, int gameIndex, int * numOfSavedGames, int * numOfPlayers) {

    char saveOption[2];
    int errorCheck = 0;

            do {
                if (errorCheck > 0) {
                    displayErrorMessage(errorCheck);
                }
                    printf("\t\t\t     Do you want to save this game?\n\n   \t\t\t          1. Yes\t2. No\n\n   \t\t\t\t      Option: ");

                    scanf(" %s", saveOption);
                    errorCheck = validateInput(saveOption, 1, 2, 1, 1, 2);
                    while (getchar() != '\n');
            } while (errorCheck);

            int saveOptionInt = atoi(saveOption);

            switch (saveOptionInt) {
                case 1: saveGame(m, n, board, boardslist, gameslist, 0, 0, numOfSavedGames, numOfPlayers); break;
                default:    break;
            }

}

void loadGame(int m, int n, char board[][n], int * winPosition, Board * boardslist, Game * gameslist, Player * playerslist, int gameIndex, int * numOfSavedGames, int * numOfPlayers) {

    int reprompt = 1;
    int errorCheck = 0;
    char option[2];



    if (!*numOfSavedGames) {
        system(CLEAR_CONSOLE);
        displayLogo();

        printf("                       	            -{ Saved Games }-\n\n"
        "*******************************************************************************************\n"
        "\n"
        "   \tGame ID\t\t X\t\t O\t\tStatus\t\tTurn\tTotal Moves\n"
        "\n"
        "*******************************************************************************************\n\n"
        );

        printf(
            "\t\t\t     Empty! There are no saved games yet.\n\n\n"
            "\t\t\t        Press Enter to Return . . . \n\n\n\t\t\t\t\t  ");
        while (getchar() != '\n');
        pressEnter();
    } else {
        system(CLEAR_CONSOLE);
        displayLogo();

        printf("                       	            -{ Saved Games }-\n\n"
        "*******************************************************************************************\n"
        "\n"
        "   \tGame ID\t\t X\t\t O\t\tStatus\t       Turn   Total Moves\n"
        "\n"
        "*******************************************************************************************\n\n"
        );

        load_Games_File(gameslist, boardslist);

            for (int i = 0; i < *numOfSavedGames; i++) {

                printf("   %d.\t%d\t\t(%d) %s",
                i+1,
                gameslist[i].GameID,
                gameslist[i].currentXpoints,
                playerslist[gameslist[i].player1ID].firstName);                
                
                if (strlen(playerslist[gameslist[i].player1ID].firstName) < 4) {
                    printf("\t\t");
                } else {
                    printf("\t");
                }

                printf("(%d) %s",
                gameslist[i].currentOpoints,
                playerslist[gameslist[i].player2ID].firstName);
                
                if (strlen(playerslist[gameslist[i].player2ID].firstName) < 4) {
                    printf("\t\t");
                } else {
                    printf("\t");
                }

                printf("%s\t\t%c\t   %d\n",
                gameslist[i].gameStatus,
                gameslist[i].whosTurn,
                gameslist[i].totalMoves
                );
            }

        do {
            if (errorCheck > 0) {
                displayErrorMessage(errorCheck);
            }
            printf("\n\n   Select a saved game by index number (0 to exit): ");

            scanf(" %s", option);
            errorCheck = validateInput(option, 2, 2, 0, 1, *numOfSavedGames);
            while (getchar() != '\n');
        } while (errorCheck != 0);

        int loadGame = atoi(option);

        if (loadGame) {
            newGame(m, n, board, winPosition, boardslist, gameslist, playerslist, numOfSavedGames, numOfPlayers, 1, loadGame);
        }
    }

}

//############################################################################################
//
//  Create Games related functions - END
//
//############################################################################################






//############################################################################################
//
//  "Leaderboard" related functions - START                                          15 points
//
//############################################################################################

void swapInt(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swapChar(char* a, char* b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void swapString(char* a, char* b) {
    char temp[25];
    strcpy(temp, a);
    strcpy(a, b);
    strcpy(b, temp);
}

int partition(Player * list, int start, int end, int sortMode) {
    int partitionIndex = start;

    if (sortMode == 1 || sortMode == 2) {
    int pivot = list[end].playerScore;

        for (int i = start; i < end; i++) {
            if (list[i].playerScore <= pivot) {
                swapString(list[i].firstName, list[partitionIndex].firstName);
                swapString(list[i].lastName, list[partitionIndex].lastName);
                swapString(list[i].playerEmail, list[partitionIndex].playerEmail);
                swapInt(&list[i].playerScore, &list[partitionIndex].playerScore);
                swapInt(&list[i].playerGames, &list[partitionIndex].playerGames);
                swapInt(&list[i].playerID, &list[partitionIndex].playerID);
                swapInt(&list[i].wins, &list[partitionIndex].wins);
                swapInt(&list[i].draws, &list[partitionIndex].draws);
                swapInt(&list[i].losses, &list[partitionIndex].losses);
                partitionIndex++;
            }
        }
        swapString(list[partitionIndex].firstName, list[end].firstName);
        swapString(list[partitionIndex].lastName, list[end].lastName);
        swapString(list[partitionIndex].playerEmail, list[end].playerEmail);
        swapInt(&list[partitionIndex].playerScore, &list[end].playerScore);
        swapInt(&list[partitionIndex].playerGames, &list[end].playerGames);
        swapInt(&list[partitionIndex].playerID, &list[end].playerID);
        swapInt(&list[partitionIndex].wins, &list[end].wins);
        swapInt(&list[partitionIndex].draws, &list[end].draws);
        swapInt(&list[partitionIndex].losses, &list[end].losses);
    }

    if (sortMode == 3 || sortMode == 4) {
    int pivot = list[end].wins;

        for (int i = start; i < end; i++) {
            if (list[i].wins <= pivot) {
                swapString(list[i].firstName, list[partitionIndex].firstName);
                swapString(list[i].lastName, list[partitionIndex].lastName);
                swapString(list[i].playerEmail, list[partitionIndex].playerEmail);
                swapInt(&list[i].playerScore, &list[partitionIndex].playerScore);
                swapInt(&list[i].playerGames, &list[partitionIndex].playerGames);
                swapInt(&list[i].playerID, &list[partitionIndex].playerID);
                swapInt(&list[i].wins, &list[partitionIndex].wins);
                swapInt(&list[i].draws, &list[partitionIndex].draws);
                swapInt(&list[i].losses, &list[partitionIndex].losses);
                partitionIndex++;
            }
        }
        swapString(list[partitionIndex].firstName, list[end].firstName);
        swapString(list[partitionIndex].lastName, list[end].lastName);
        swapString(list[partitionIndex].playerEmail, list[end].playerEmail);
        swapInt(&list[partitionIndex].playerScore, &list[end].playerScore);
        swapInt(&list[partitionIndex].playerGames, &list[end].playerGames);
        swapInt(&list[partitionIndex].playerID, &list[end].playerID);
        swapInt(&list[partitionIndex].wins, &list[end].wins);
        swapInt(&list[partitionIndex].draws, &list[end].draws);
        swapInt(&list[partitionIndex].losses, &list[end].losses);
    }

    if (sortMode == 5 || sortMode == 6) {
    int pivot = list[end].draws;

        for (int i = start; i < end; i++) {
            if (list[i].draws <= pivot) {
                swapString(list[i].firstName, list[partitionIndex].firstName);
                swapString(list[i].lastName, list[partitionIndex].lastName);
                swapString(list[i].playerEmail, list[partitionIndex].playerEmail);
                swapInt(&list[i].playerScore, &list[partitionIndex].playerScore);
                swapInt(&list[i].playerGames, &list[partitionIndex].playerGames);
                swapInt(&list[i].playerID, &list[partitionIndex].playerID);
                swapInt(&list[i].wins, &list[partitionIndex].wins);
                swapInt(&list[i].draws, &list[partitionIndex].draws);
                swapInt(&list[i].losses, &list[partitionIndex].losses);
                partitionIndex++;
            }
        }
        swapString(list[partitionIndex].firstName, list[end].firstName);
        swapString(list[partitionIndex].lastName, list[end].lastName);
        swapString(list[partitionIndex].playerEmail, list[end].playerEmail);
        swapInt(&list[partitionIndex].playerScore, &list[end].playerScore);
        swapInt(&list[partitionIndex].playerGames, &list[end].playerGames);
        swapInt(&list[partitionIndex].playerID, &list[end].playerID);
        swapInt(&list[partitionIndex].wins, &list[end].wins);
        swapInt(&list[partitionIndex].draws, &list[end].draws);
        swapInt(&list[partitionIndex].losses, &list[end].losses);
    }

    if (sortMode == 7 || sortMode == 8) {
    int pivot = list[end].losses;

        for (int i = start; i < end; i++) {
            if (list[i].losses <= pivot) {
                swapString(list[i].firstName, list[partitionIndex].firstName);
                swapString(list[i].lastName, list[partitionIndex].lastName);
                swapString(list[i].playerEmail, list[partitionIndex].playerEmail);
                swapInt(&list[i].playerScore, &list[partitionIndex].playerScore);
                swapInt(&list[i].playerGames, &list[partitionIndex].playerGames);
                swapInt(&list[i].playerID, &list[partitionIndex].playerID);
                swapInt(&list[i].wins, &list[partitionIndex].wins);
                swapInt(&list[i].draws, &list[partitionIndex].draws);
                swapInt(&list[i].losses, &list[partitionIndex].losses);
                partitionIndex++;
            }
        }
        swapString(list[partitionIndex].firstName, list[end].firstName);
        swapString(list[partitionIndex].lastName, list[end].lastName);
        swapString(list[partitionIndex].playerEmail, list[end].playerEmail);
        swapInt(&list[partitionIndex].playerScore, &list[end].playerScore);
        swapInt(&list[partitionIndex].playerGames, &list[end].playerGames);
        swapInt(&list[partitionIndex].playerID, &list[end].playerID);
        swapInt(&list[partitionIndex].wins, &list[end].wins);
        swapInt(&list[partitionIndex].draws, &list[end].draws);
        swapInt(&list[partitionIndex].losses, &list[end].losses);
    }

    if (sortMode == 9 || sortMode == 10) {
    int pivot = list[end].playerGames;

        for (int i = start; i < end; i++) {
            if (list[i].playerGames <= pivot) {
                swapString(list[i].firstName, list[partitionIndex].firstName);
                swapString(list[i].lastName, list[partitionIndex].lastName);
                swapString(list[i].playerEmail, list[partitionIndex].playerEmail);
                swapInt(&list[i].playerScore, &list[partitionIndex].playerScore);
                swapInt(&list[i].playerGames, &list[partitionIndex].playerGames);
                swapInt(&list[i].playerID, &list[partitionIndex].playerID);
                swapInt(&list[i].wins, &list[partitionIndex].wins);
                swapInt(&list[i].draws, &list[partitionIndex].draws);
                swapInt(&list[i].losses, &list[partitionIndex].losses);
                partitionIndex++;
            }
        }
        swapString(list[partitionIndex].firstName, list[end].firstName);
        swapString(list[partitionIndex].lastName, list[end].lastName);
        swapString(list[partitionIndex].playerEmail, list[end].playerEmail);
        swapInt(&list[partitionIndex].playerScore, &list[end].playerScore);
        swapInt(&list[partitionIndex].playerGames, &list[end].playerGames);
        swapInt(&list[partitionIndex].playerID, &list[end].playerID);
        swapInt(&list[partitionIndex].wins, &list[end].wins);
        swapInt(&list[partitionIndex].draws, &list[end].draws);
        swapInt(&list[partitionIndex].losses, &list[end].losses);
    }

    return partitionIndex;
}

void quicksort_asc(Player* list, int start, int end, int sortMode) {
    if (start < end) {
        int partitionIndex = partition(list, start, end, sortMode);
        quicksort_asc(list, start, partitionIndex-1, sortMode);
        quicksort_asc(list, partitionIndex+1, end, sortMode);
    }
}

void quicksort_des(Player* list, int start, int end) {

    Player * temp = (Player *) malloc((end+1)*sizeof(Player));

    // copy to a temp array
    for (int i = start; i <= end; i++) {
        strcpy(temp[i].firstName, list[i].firstName);
        strcpy(temp[i].lastName, list[i].lastName);
        strcpy(temp[i].playerEmail, list[i].playerEmail);
        temp[i].playerScore = list[i].playerScore;
        temp[i].playerGames = list[i].playerGames;
        temp[i].playerID = list[i].playerID;
        temp[i].wins = list[i].wins;
        temp[i].draws = list[i].draws;
        temp[i].losses = list[i].losses;
    }

    // re-assign the elements in reverse order using data in temp array
    for (int i = start, j = end; i <= end; i++, j--) {
        strcpy(list[i].firstName, temp[j].firstName);
        strcpy(list[i].lastName, temp[j].lastName);
        strcpy(list[i].playerEmail, temp[j].playerEmail);
        list[i].playerScore = temp[j].playerScore;
        list[i].playerGames = temp[j].playerGames;
        list[i].playerID = temp[j].playerID;
        list[i].wins = temp[j].wins;
        list[i].draws = temp[j].draws;
        list[i].losses = temp[j].losses;
    }
    free(temp);
}

void leaderBoard(Player * playerslist, int numOfPlayers) {

    int sortMode = 1;
    char sortOption[3];
    int errorCheck = 0;
    int reprompt = 1;

    Player * templist = (Player *) malloc(numOfPlayers*sizeof(Player));


    if (!numOfPlayers) {
        system(CLEAR_CONSOLE);
        displayLogo();

        printf("                       	            -{ Leaderboard }-\n\n"
        "*******************************************************************************************\n"
        "\n"
        "   Rank  |  ID\t|  Name\t\t\t| Score | Wins  | Draws | Losses| Total Games\n"
        "\n"
        "*******************************************************************************************\n\n"
        "\n\n"
        );

        printf(
            "\t\t\t     Empty! There are no players yet.\n\n\n"
            "\t\t\t       Press Enter to Return . . . \n\n\n\t\t\t\t\t  ");
            pressEnter();
    } else {        
    

        do {

        load_Players_File(playerslist);
        copyList(playerslist, templist, numOfPlayers);

        system(CLEAR_CONSOLE);
        displayLogo();


        printf("                       	            -{ Leaderboard }-\n\n"
        "*******************************************************************************************\n"
        "\n"
        "   Rank  |  ID\t|  Name\t\t\t| Score | Wins  | Draws | Losses| Total Games\n"
        "\n"
        "*******************************************************************************************\n\n"
        "   Currently sorted by: %d\n\n", sortMode
        );

            if ((sortMode % 2) == 1) {
                quicksort_asc(templist, 0, numOfPlayers-1, sortMode);
                quicksort_des(templist, 0, numOfPlayers-1);  // reverse the sorted array for a decending view
            } else {
                quicksort_asc(templist, 0, numOfPlayers-1, sortMode);
            }

            for (int i = 0; i < numOfPlayers; i++) {
                printf("   %d\t |  %d\t| %s %s",
                    i+1,
                    templist[i].playerID,
                    templist[i].firstName,
                    templist[i].lastName
                );
                    if ((strlen(templist[i].firstName) + strlen(templist[i].lastName)) > 12) {
                        printf("\t");
                    } else {
                        printf("\t\t");
                    }
                printf("|  %d\t|  %d\t|  %d\t|  %d\t|  %d\n",
                    templist[i].playerScore,
                    templist[i].wins,
                    templist[i].draws,
                    templist[i].losses,
                    templist[i].playerGames
                );
            }
        

            do {
                if (errorCheck > 0) {
                    displayErrorMessage(errorCheck);
                }
                    printf("\n\n   Available sort options:\n"
                    "   1  Sort by Score in decending order\n"
                    "   2  Sort by Score in ascending order\n"
                    "   3  Sort by Wins in decending order\n"
                    "   4  Sort by Wins in ascending order\n"
                    "   5  Sort by Draws in decending order\n"
                    "   6  Sort by Draws in ascending order\n"
                    "   7  Sort by Losses in decending order\n"
                    "   8  Sort by Losses in ascending order\n"
                    "   9  Sort by Games in decending order\n"
                    "   10 Sort by Games in ascending order\n"
                    "   \n"
                    "   e  Exit\n\n"
                    "   Option: ");

                    scanf("%s", sortOption);
                    while (getchar() != '\n');
                    errorCheck = validateInput(sortOption, 2, 12, 0, 0, 0);
                
            } while (errorCheck);

            if (sortOption[0] == 'e' || sortOption[0] == 'E') {
                reprompt = 0;
            } else {
                sortMode = atoi(sortOption);
            }

        } while (reprompt);

    }

}

void copyList(Player * source, Player * des, int listSize) {

    for (int i = 0; i < listSize; i++) {
        strcpy(des[i].firstName, source[i].firstName);
        strcpy(des[i].lastName, source[i].lastName);
        strcpy(des[i].playerEmail, source[i].playerEmail);
        des[i].playerScore = source[i].playerScore;
        des[i].playerGames = source[i].playerGames;
        des[i].playerID = source[i].playerID;
        des[i].wins = source[i].wins;
        des[i].draws = source[i].draws;
        des[i].losses = source[i].losses;
    }

}

//############################################################################################
//
//  "Leaderboard" related functions - END
//
//############################################################################################




//############################################################################################
//
//  Helper functions - START
//
//############################################################################################

void displayLogo() {
    printf(
    "___________.____________   ________________  _________   ___________________  ___________\n"
    "\\__    ___/|   \\_   ___ \\  \\__    ___/  _  \\ \\_   ___ \\  \\__    ___/\\_____  \\ \\_   _____/\n"
    "  |    |   |   /    \\  \\/    |    | /  /_\\  \\/    \\  \\/    |    |    /   |   \\ |    __)_ \n"
    "  |    |   |   \\     \\____   |    |/    |    \\     \\____   |    |   /    |    \\|        \\\n"
    "  |____|   |___|\\______  /   |____|\\____|__  /\\______  /   |____|   \\_______  /_______  /\n"
    "                       \\/                  \\/        \\/                     \\/        \\/ \n"
    "                                                                           by Kap Thang\n\n"
    );
}

void displayMainMenu() {

        printf(
        "   >> Main Menu\n\n"
        "   press 'p' to add a new player\n"
        "   press 'g' to play a new game or load a saved game\n"
        "   press 'l' to display the leaderboard\n"
        "   press 'r' to reset all the cells\n"         // an extra option I added. this was very useful during testing. I don't need to exit and rerun the program each time I wanted to try different scenarios
        "   press 'e' to Exit\n\n");
}

void pressEnter() {
    int ch;
    while ((ch = getchar() != '\n'));
}

int getNumOf(int option) {

    FILE * db = fopen("database.txt", "r");

    int numOfPlayers;
    int numOfSavedGames;

    if (db == NULL) {
        return 1;
    }

    fscanf(db, READ_DATABASE_FILE, &numOfPlayers, &numOfSavedGames);
    
    fclose(db);

    switch (option) {
        case 1: return numOfPlayers; break;
        case 2: return numOfSavedGames; break;
        default: return 0;
    }

    
    return 0;
}

int inputOption(char str[], int n) {
    int ch, i = 0;
    while ((ch = getchar()) != '\n')
        if (i < n)
            str[i++] = ch;
    str[i] = '\0';
    return i;
}

void displayErrorMessage(int errorCode) {
    switch (errorCode) {
        case 1: printf("\n\tThe input exceeds maximum character limit!.\n\n"); break;
        case 2: printf("\n\tInvalid input! Enter only numbers within the range (if any).\n\n"); break;
        case 3: printf("\n\tInvalid input! Enter only lowercase letters.\n\n"); break;
        case 4: printf("\n\tInvalid input! Enter only uppercase letters.\n\n"); break;
        case 5: printf("\n\tInvalid input! Enter only letters.\n\n"); break;
        case 6: printf("\n\tInvalid input! Enter only numbers, letters and some special characters only\n\n"); break;
        case 7: printf("\n\tInvalid email! Try again.\n\n"); break;
        case 10: printf("\n\tError! Cell is already occupied. Pick another cell.\n\n"); break;
        case 11: printf("\n\tError! Only between 1-9 or one of the available options.\n\n"); break;
        case 12: printf("\n\tInvalid input! Enter only the available sort options or e to exit.\n\n"); break;
        case 13: printf("\n\tError! Only between 0-2.\n\n"); break;
        
    }
}

int getRandomNum(int min, int max) {
    srand(time(0));
    return rand() % max + 1;
}

int validateInput(char * str, int maxChar, int criteriaCode, int useLimits, int lowerLimit, int upperLimit) {

    /*
    Criteria codes:
    1   -
    2   numbers only
    3   lower letters only
    4   upper letters only
    5   lower and upper letters only
    6   numbers, lower and upper letters, and special characters !@#$%
    7   email address validation (no dash, 1 @, 1 dot)
    */

    /*
    Return codes:
    1   input exceeds max char limit
    2   input is limited to numbers only, input contains other chars
    3   input is limited to lower letters only, input contains other chars
    4   input is limited to upper letters only, input contains other chars
    5   input is limited to numbers, lower and upper letters only, input contains other chars
    6   input is limited to numbers, lower and upper letters, some special chars only, input contains other chars
    7   invalid email address

    */

    int charLen = 0;
    while (str[charLen] != '\0') {
        charLen++;
    }
   
   if (charLen > maxChar) {
       return 1;
   } else if (criteriaCode == 2) {
       for (int i = 0; i < charLen; i++) {
           if (str[i] < '0' || str[i] > '9') {
               return 2;
           }
           int num = atoi(str);
           if (useLimits == 1 && (num < lowerLimit || num > upperLimit)) {
               return 2;
           }
       }

   } else if (criteriaCode == 11) {
       for (int i = 0; i < charLen; i++) {
           if ((str[i] < '1' || str[i] > '9') && str[i] != 'h' && str[i] != 'H' && str[i] != 's' && str[i] != 'S' && str[i] != 'e' && str[i] != 'E') {
               return 11;
           }
       }
       
    } else if (criteriaCode == 3) {
       for (int i = 0; i < charLen; i++) {
           if (str[i] < 'a' || str[i] > 'z') {
               return 3;
           }
       }
   } else if (criteriaCode == 4) {
       for (int i = 0; i < charLen; i++) {
           if (str[i] < 'A' || str[i] > 'Z') {
               return 4;
           }
       }
   } else if (criteriaCode == 5) {
       for (int i = 0; i < charLen; i++) {
           if ((str[i] < 'A' || str[i] > 'Z') || (str[i] < 'a' || str[i] > 'z')) {
               return 5;
           }
       }
   } else if (criteriaCode == 6) {
       for (int i = 0; i < charLen; i++) {
           if ((str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z') && 
           (str[i] != '!' && str[i] != '@' && str[i] != '#' && str[i] != '$' && str[i] != '%')) {
               return 6;
           }
       }
   } else if (criteriaCode == 12) {
       int num = atoi(str);
       for (int i = 0; i < charLen; i++) {
           if ((num < 1 || num > 10) && str[i] != 'e' && str[i] != 'E') {
               return 12;
           }
       }

   } else if (criteriaCode == 13) {
       int num = atoi(str);
       for (int i = 0; i < charLen; i++) {
           if (num < 0 || num > 2) {
               return 13;
           }
       }

   } else if (criteriaCode == 7) {

       int addCounter = 0, dotCounter = 0, plusCounter = 0;
       for (int i = 0; i < charLen; i++) {
           if ((str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z') && (str[i] != '@' && str[i] != '.' && str[i] != '+')) {
               return 7;
           }
           if (str[i] == '@' && addCounter < 1) {
               addCounter++;
           } else if (str[i] == '@' && addCounter > 0) {
               return 7;
           }
           if (str[i] == '.' && dotCounter < 1) {
               dotCounter++;
           } else if (str[i] == '.' && dotCounter > 0) {
               return 7;
           }
           if (str[i] == '+' && plusCounter < 1) {
               plusCounter++;
           } else if (str[i] == '+' && plusCounter > 0) {
               return 7;
           }
           if ((i == charLen-1 && addCounter < 1) || (i == charLen - 1 && dotCounter < 1)) {
               return 7;
           }
       }
   }
   return 0;    // 0 == all good
}

int overwrite_Players_File(Player * playerslist, int * numOfPlayers) {
            FILE * fp = fopen("players.txt", "w+");

            if (fp == NULL) {
                return 1;
            }

            for (int i = 0; i < *numOfPlayers; i++) {
                fprintf(fp, WRITE_PLAYERS_FILE,
                playerslist[i].playerID,
                playerslist[i].firstName,
                playerslist[i].lastName,
                playerslist[i].playerEmail,
                playerslist[i].playerScore,
                playerslist[i].wins,
                playerslist[i].draws,
                playerslist[i].losses,
                playerslist[i].playerGames
                );
            }
            fclose(fp);
    return 0;
}

int overwrite_Games_File(Game * gameslist, Board * boardslist, int * numOfSavedGames) {
            FILE * fp = fopen("savedgames.txt", "w+");

            if (fp == NULL) {
                return 1;
            }

            for (int i = 0; i < *numOfSavedGames; i++) {
                fprintf(fp, WRITE_GAMES_FILE,
                gameslist[i].GameID,
                boardslist[i].boardIndex,
                gameslist[i].gameStatus,
                gameslist[i].player1ID,
                gameslist[i].currentXpoints,
                gameslist[i].player2ID,
                gameslist[i].currentOpoints,
                gameslist[i].whosTurn,
                gameslist[i].totalMoves
                );

            }
            fclose(fp);
    return 0;
}

int overwrite_Boards_File(Board * boardslist, int * numOfSavedGames) {

    FILE * fp = fopen("savedboards.txt", "w+");

    if (fp == NULL) {
        return 1;
    }

    for (int i = 0; i < *numOfSavedGames; i++) {
        fprintf(fp, WRITE_BOARDS_FILE,
            boardslist[i].boardIndex,
            boardslist[i].board[0],
            boardslist[i].board[1],
            boardslist[i].board[2],
            boardslist[i].board[3],
            boardslist[i].board[4],
            boardslist[i].board[5],
            boardslist[i].board[6],
            boardslist[i].board[7],
            boardslist[i].board[8]
            );
    }
    fclose(fp);
    return 0;
}

int append_Players_File(Player * playerslist, int index) {
            FILE * fp = fopen("players.txt", "a+");

            if (fp == NULL) {
                return 1;
            }

            
            fprintf(fp, WRITE_PLAYERS_FILE,
                playerslist[index].playerID,
                playerslist[index].firstName,
                playerslist[index].lastName,
                playerslist[index].playerEmail,
                playerslist[index].playerScore,
                playerslist[index].wins,
                playerslist[index].draws,
                playerslist[index].losses,
                playerslist[index].playerGames
                );
            fclose(fp);
    return 0;
}

int append_Games_File(Game * gameslist, Board * boardslist, int index) {
        FILE * fp = fopen("savedgames.txt", "a+");

        if (fp == NULL) {
            return 1;
        }

        fprintf(fp, WRITE_GAMES_FILE,
            gameslist[index].GameID,
            boardslist[index].boardIndex,
            gameslist[index].gameStatus,
            gameslist[index].player1ID,
            gameslist[index].currentXpoints,
            gameslist[index].player2ID,
            gameslist[index].currentOpoints,
            gameslist[index].whosTurn,
            gameslist[index].totalMoves
            );
        fclose(fp);
    return 0;
}

int append_Boards_File(Board * boardslist, int index) {
        FILE * fp = fopen("savedboards.txt", "a+");

        if (fp == NULL) {
            return 1;
        }
        fprintf(fp, WRITE_BOARDS_FILE,
            boardslist[index].boardIndex,
            boardslist[index].board[0],
            boardslist[index].board[1],
            boardslist[index].board[2],
            boardslist[index].board[3],
            boardslist[index].board[4],
            boardslist[index].board[5],
            boardslist[index].board[6],
            boardslist[index].board[7],
            boardslist[index].board[8]
            );
        fclose(fp);
    return 0;
}

int update_Database_File(int * numOfPlayers, int * numOfSavedGames) {
        FILE * fp = fopen("database.txt", "w+");

        if (fp == NULL) {
            return 1;
        }

        fprintf(fp, WRITE_DATABASE_FILE,
            *numOfPlayers,
            *numOfSavedGames
            );
        fclose(fp);
    return 0;
}

int load_Boards_File(Board * boardslist) {
        FILE * fp = fopen("savedboards.txt", "r");

        if (fp == NULL) {
            return 1;
        }

        int i = 0;
        while (!feof(fp)) {
            fscanf(fp, READ_BOARDS_FILE,
                &boardslist[i].boardIndex,
                &boardslist[i].board[0],
                &boardslist[i].board[1],
                &boardslist[i].board[2],
                &boardslist[i].board[3],
                &boardslist[i].board[4],
                &boardslist[i].board[5],
                &boardslist[i].board[6],
                &boardslist[i].board[7],
                &boardslist[i].board[8]
                );
            i++;
        }
        fclose(fp);
    return 0;
}

int load_Games_File(Game * gameslist, Board * boardslist) {
        FILE * fp = fopen("savedgames.txt", "r");

        if (fp == NULL) {
            return 1;
        }

        int i = 0;
        while (!feof(fp)) {
            fscanf(fp, READ_GAMES_FILE,
                &gameslist[i].GameID,
                &boardslist[i].boardIndex,
                gameslist[i].gameStatus,
                &gameslist[i].player1ID,
                &gameslist[i].currentXpoints,
                &gameslist[i].player2ID,
                &gameslist[i].currentOpoints,
                &gameslist[i].whosTurn,
                &gameslist[i].totalMoves
                );
            i++;
        }
        fclose(fp);
    return 0;
}

int load_Players_File(Player * playerslist) {
        FILE * fp = fopen("players.txt", "r");

        if (fp == NULL) {
            return 1;
        }

        int i = 0;
        while (!feof(fp)) {
            fscanf(fp, READ_PLAYERS_FILE,
                &playerslist[i].playerID,
                playerslist[i].firstName,
                playerslist[i].lastName,
                playerslist[i].playerEmail,
                &playerslist[i].playerScore,
                &playerslist[i].wins,
                &playerslist[i].draws,
                &playerslist[i].losses,
                &playerslist[i].playerGames
                );
            i++;
        }
        fclose(fp);
    return 0;
}

//############################################################################################
//
//  Helper functions - END
//
//############################################################################################







//############################################################################################
//
//  Core functions from ASSIGNMENT 1   - START
//
//############################################################################################

void caseOne(char player, char singleList[]) {
    printf("\n   Cell %c is a winning cell for player '%c'", singleList[0], player);
}

void caseTwo(char player, char singleList[]) {
    printf("\n   Cells %c and %c are winning cells for player '%c'", singleList[0], singleList[1], player);
}

void caseThree(char player, char singleList[]) {
    printf("\n   Cells %c, %c, and %c are winning cells for player '%c'", singleList[0], singleList[1], singleList[2], player);
}

int gameStillRunning(int m, int n, char board[][n], int * winPosition) {

    // returns 1 if there are still moves available and no on has won yet
    return ((getMovesNumber(m, n, board) < m * n) && whoIsTheWinner(m, n, board, winPosition) != 'X' &&  whoIsTheWinner(m, n, board, winPosition) != 'O') ? 1 : 0;
}

int getMovesNumber(int m, int n, char board[][n]) {

    int movesNumber = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'X' || board[i][j] == 'O') {
                movesNumber++;
            }
        }
    }

    return movesNumber;
}

void checkValid(int m, int n, char board[][n]) {

    printBoard(m, n, board, 1);

    if (isValidBoard(m, n, board)) {
        printf("  Valid Board\n");
    } else {
        printf("  Invalid Board\n");
    }
}

void displayFinalBoard(int m, int n, char board[][n], int * winPosition, Player * playerslist, Game * gameslist, int thisGame, char winner) {

    int rows[3];
    int cols[3];

    if (*winPosition == 1) {
        rows[0] = rows[1] = rows[2] = 0;
        cols[0] = 0, cols[1] = 1, cols[2] = 2;
    }

    if (*winPosition == 2) {
        rows[0] = rows[1] = rows[2] = 1;
        cols[0] = 0, cols[1] = 1, cols[2] = 2;
    }

    if (*winPosition == 3) {
        rows[0] = rows[1] = rows[2] = 2;
        cols[0] = 0, cols[1] = 1, cols[2] = 2;
    }

    if (*winPosition == 4) {
        cols[0] = cols[1] = cols[2] = 0;
        rows[0] = 0, rows[1] = 1, rows[2] = 2;
    }

    if (*winPosition == 5) {
        cols[0] = cols[1] = cols[2] = 1;
        rows[0] = 0, rows[1] = 1, rows[2] = 2;
    }

    if (*winPosition == 6) {
        cols[0] = cols[1] = cols[2] = 2;
        rows[0] = 0, rows[1] = 1, rows[2] = 2;
    }

    if (*winPosition == 7) {
        rows[0] = 0, rows[1] = 1, rows[2] = 2;
        cols[0] = 0, cols[1] = 1, cols[2] = 2;
    }

    if (*winPosition == 8) {
        rows[0] = 0, rows[1] = 1, rows[2] = 2;
        cols[0] = 2, cols[1] = 1, cols[2] = 0;
    }




    system(CLEAR_CONSOLE);
    displayLogo();

    printf("\t\t\t\t  %s (X) Vs. %s (O)\n\n",
    playerslist[gameslist[thisGame].player1ID].firstName,
    playerslist[gameslist[thisGame].player2ID].firstName    
    );



    int k = 0;
    for (int i = 0; i < m; i++) {

        printf("\t\t\t\t       |       |       \n\t\t\t\t");    // print first empty line
        for (int j = 0; j < n; j++) {

            if (rows[k] == i && cols[k] == j) {
                printf("  {%c}  ", board[i][j]);
                k++;
            } else {
                printf("   %c   ", board[i][j]); 
        }
            
            if (j < n-1) {
                printf("|");                    // print separator 2 times each row
            }
        }
        printf("\n");
            if (i < m-1) {
                printf("\t\t\t\t_______|_______|_______\n");
            }
        if (i == m-1) {
            printf("\t\t\t\t       |       |       \n\n");  // final print of empty line
        }
    }






    switch (winner) {
        case 'X': printf("\t\t\t\t    Player X Wins!\n\n"); break;
        case 'O': printf("\t\t\t\t    Player O Wins!\n\n"); break;
        case 'D': printf("\t\t\t\t   No winner. A draw.\n\n"); break;
    }

    int deltaXPoints = 0;
    int deltaOPoints = 0;
    int updatedXPoints;
    int updatedOPoints;
    char xSign = ' ';
    char oSign = ' ';

    if (winner == 'X' && playerslist[gameslist[thisGame].player2ID].playerScore > playerslist[gameslist[thisGame].player1ID].playerScore) {
        updatedXPoints = playerslist[gameslist[thisGame].player1ID].playerScore + gameslist[thisGame].currentXpoints;
        updatedOPoints = playerslist[gameslist[thisGame].player2ID].playerScore - 2;
        deltaXPoints += gameslist[thisGame].currentXpoints;
        deltaOPoints -= 2 * (-1);
        xSign = '+';
        oSign = '-';
    } else if (winner == 'X' && playerslist[gameslist[thisGame].player2ID].playerScore <= playerslist[gameslist[thisGame].player1ID].playerScore) {
        updatedXPoints = playerslist[gameslist[thisGame].player1ID].playerScore + gameslist[thisGame].currentXpoints;
        updatedOPoints = playerslist[gameslist[thisGame].player2ID].playerScore - 1;
        deltaXPoints += gameslist[thisGame].currentXpoints;
        deltaOPoints -= 1 * (-1);
        xSign = '+';
        oSign = '-';
    }

    if (winner == 'O' && playerslist[gameslist[thisGame].player1ID].playerScore > playerslist[gameslist[thisGame].player2ID].playerScore) {
        updatedOPoints = playerslist[gameslist[thisGame].player2ID].playerScore + gameslist[thisGame].currentOpoints;
        updatedXPoints = playerslist[gameslist[thisGame].player1ID].playerScore - 2;
        deltaOPoints += gameslist[thisGame].currentOpoints;
        deltaXPoints -= 2 * (-1);
        oSign = '+';
        xSign = '-';
    } else if (winner == 'O' && playerslist[gameslist[thisGame].player1ID].playerScore <= playerslist[gameslist[thisGame].player2ID].playerScore) {
        updatedOPoints = playerslist[gameslist[thisGame].player2ID].playerScore + gameslist[thisGame].currentOpoints;
        updatedXPoints = playerslist[gameslist[thisGame].player1ID].playerScore - 1;
        deltaOPoints += gameslist[thisGame].currentOpoints;
        deltaXPoints -= 1 * (-1);
        oSign = '+';
        xSign = '-';
    }

    if (winner == 'D') {
        printf(
            "   >> Updated player scores:\t      X: %d points\t\t     Game Status: %s\n\t\t\t\t      O: %d points"
            "\t\t\t  Move #: %d\n\n",
            playerslist[gameslist[thisGame].player1ID].playerScore,
            gameslist[thisGame].gameStatus,
            playerslist[gameslist[thisGame].player2ID].playerScore,
            gameslist[thisGame].totalMoves
        );
    } else {
        printf(
            "   >> Updated player scores:\t      X: %d (%c%d)\t\t     Game Status: %s\n\t\t\t\t      O: %d (%c%d)"
            "\t\t\t  Move #: %d\n\n",
            updatedXPoints,
            xSign,
            deltaXPoints,
            gameslist[thisGame].gameStatus,
            updatedOPoints,
            oSign,
            deltaOPoints,
            gameslist[thisGame].totalMoves
        );
    }

    printf("\n\n");


}

int searchAndSwapCellValue(int position, int m, int n, char turn, char board[][n]) {

    int x;
    int traversePosition = 0;
    // linear search

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (traversePosition == (position-1)) {
                if (position == board[i][j] - '0') {
                    board[i][j] = turn;
                    x = 1;
                    i = 3;
                    j = 3;
                } else {
                    x = 0;
                    i = 3;
                    j = 3;
                }
            } else {
                traversePosition++;
            }
        }
    }

    return x;
}

char whoIsTheWinner(int m, int n, char board[][n], int * winPosition) {

    char thePlayers[2] = {'X', 'O'};
    char theWinner = 'D';       // will remain as D if no one wins
    int keepScanning = 1;       // tracker to keep scanning until a winner is found

    for (int k = 0; k < 2; k++) {

        if (keepScanning) {

            // Horizontal scan
            for (int i = 0, countCells = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    if (board[i][j] == thePlayers[k]) {
                        countCells++;
                    }
                    if (countCells == 3) {
                        switch (i) {
                            case 0: *winPosition += 1; break;
                            case 1: *winPosition += 2; break;
                            case 2: *winPosition += 3; break;
                        }
                        j = 3;
                        i = 3;
                        theWinner = thePlayers[k];
                        keepScanning = 0;
                        k = 2;      // make k = 2 to stop scanning for the other player
                    } else if (countCells < 3 && j == n-1) {
                        countCells = 0;
                    }
                }
            }
        }

        if (keepScanning) {

            // Vertical scan
            for (int i = 0, countCells = 0; i < m; i++) {
                for (int j = 0; j < n; j++) {
                    if (board[j][i] == thePlayers[k]) {
                        countCells++;
                    }
                    if (countCells == 3) {
                        switch (i) {
                            case 0: *winPosition += 4; break;
                            case 1: *winPosition += 5; break;
                            case 2: *winPosition += 6; break;
                        }
                        j = 3;
                        i = 3;
                        theWinner = thePlayers[k];
                        keepScanning = 0;
                        k = 2;      // make k = 2 to stop scanning for the other player
                    } else if (countCells < 3 && j == n-1) {
                        countCells = 0;
                    }
                }
            }
        }

        if (keepScanning) {

        // Forward Diagonal scan 
            for (int i = 0, j = 0, countCells = 0; i < m; i++, j++) {

                if (board[j][i] == thePlayers[k]) {
                    countCells++;
                }
                if (countCells == 3) {
                    *winPosition += 7;
                    j = 3;
                    i = 3;
                    theWinner = thePlayers[k];
                    keepScanning = 0;
                    k = 2;      // make k = 2 to stop scanning for the other player
                } else if (countCells < 3 && i == m-1) {
                    countCells = 0;
                }
                
            }
        }


        if (keepScanning) {

        // Reversed Diagonal scan 
            for (int i = m-1, j = 0, countCells = 0; i >= 0; i--, j++) {

                if (board[i][j] == thePlayers[k]) {
                    countCells++;
                }
                if (countCells == 3) {
                    *winPosition += 8;
                    j = 3;
                    i = -1;
                    theWinner = thePlayers[k];
                    keepScanning = 0;
                    k = 2;      // make k = 2 to stop scanning for the other player
                } else if (countCells < 3 && j == n-1) {
                    countCells = 0;
                }
                
            }

        }

    }
    return theWinner;
}

void listWinningCells(char turn, int m, int n, char board[][n]) {

    // system(CLEAR_CONSOLE);

    // printBoard(m, n, board);

    // Check validity of board before making predictions

    if (isValidBoard(m, n, board)) {

        int countX = 0;
        int countO = 0;
        int countEmpty = 0;
        char moveOption;

        char xWinningCells[4] = {0};
        char oWinningCells[4] = {0};
        char sameWinningCells[4] = {0};

        int xWinningCellsCounter = 0;
        int oWinningCellsCounter = 0;
        int sameWinningCellsCounter = 0;

        // Horizontal scan

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == 'X') {
                    countX++;
                } else if (board[i][j] == 'O') {
                    countO++;
                } else {
                    countEmpty++;
                    moveOption = board[i][j];
                }
            }

            if (countEmpty == 1 && (countX > countO)) {
                xWinningCells[xWinningCellsCounter] = moveOption;
                xWinningCellsCounter++;
            } else if (countEmpty == 1 && (countX < countO)) {
                oWinningCells[oWinningCellsCounter] = moveOption;
                oWinningCellsCounter++;
            }

            countX = 0, countO = 0, countEmpty = 0; // reset the counters before going to next row
        }

        // Vertical scan

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (board[j][i] == 'X') {
                    countX++;
                } else if (board[j][i] == 'O') {
                    countO++;
                } else {
                    countEmpty++;
                    moveOption = board[j][i];
                }
            }

            if (countEmpty == 1 && (countX > countO)) {
                xWinningCells[xWinningCellsCounter] = moveOption;
                xWinningCellsCounter++;
            } else if (countEmpty == 1 && (countX < countO)) {
                oWinningCells[oWinningCellsCounter] = moveOption;
                oWinningCellsCounter++;
            }

            countX = 0, countO = 0, countEmpty = 0; // reset the counters before going to next row
        }


        // Fordward Diagonal scan

        for (int i = 0, j = 0; i < m; i++, j++) {
            
            if (board[i][j] == 'X') {
                countX++;
            } else if (board[i][j] == 'O') {
                countO++;
            } else {
                countEmpty++;
                moveOption = board[i][j];
            }
              
        }

            if (countEmpty == 1 && (countX > countO)) {
                xWinningCells[xWinningCellsCounter] = moveOption;
                xWinningCellsCounter++;
            } else if (countEmpty == 1 && (countX < countO)) {
                oWinningCells[oWinningCellsCounter] = moveOption;
                oWinningCellsCounter++;
            }

        countX = 0, countO = 0, countEmpty = 0; // reset the counters before going to next row


        // Reversed Diagonal scan

        for (int i = m-1, j = 0; i >= 0; i--, j++) {
            
            if (board[i][j] == 'X') {
                countX++;
            } else if (board[i][j] == 'O') {
                countO++;
            } else {
                countEmpty++;
                moveOption = board[i][j];
            }
                 
        }
            if (countEmpty == 1 && (countX > countO)) {
                xWinningCells[xWinningCellsCounter] = moveOption;
                xWinningCellsCounter++;
            } else if (countEmpty == 1 && (countX < countO)) {
                oWinningCells[oWinningCellsCounter] = moveOption;
                oWinningCellsCounter++;
            }

        countX = 0, countO = 0, countEmpty = 0; // reset the counters before going to next row

        // Start printing the appropriate texts

        if (turn == 'X') {
            switch (xWinningCellsCounter) {
                case 1: caseOne('X', xWinningCells); break;
                case 2: caseTwo('X', xWinningCells); break;
                case 3: caseThree('X', xWinningCells); break;
                default: printf("\n   No winning cells found for player '%c'", turn); break;
            }
        } else if (turn == 'O') {
            switch (oWinningCellsCounter) {
                case 1: caseOne('O', oWinningCells); break;
                case 2: caseTwo('O', oWinningCells); break;
                case 3: caseThree('O', oWinningCells); break;
                default: printf("\n   No winning cells found for player '%c'", turn); break;
            }
        }

        printf("\n\n");

    } else {
        printf("\n   Error! Board is invalid. Cannot make predictions.\n\n");
    }

}

int isValidBoard(int m, int n, char board[][n]) {

    int numOfXs = 0;
    int numOfOs = 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'X') {
                numOfXs++;
            } else if (board[i][j] == 'O') {
                numOfOs++;
            }
        }
    }

    return ((numOfOs - numOfXs) == 1 || (numOfOs - numOfXs) == -1 || numOfOs == numOfXs || (numOfXs == 0 && numOfOs == 0)) ? 1 : 0;
}

void printBoard(int m, int n, char board[][n], int clearConsole) {

    if (clearConsole) {
        system(CLEAR_CONSOLE);
        displayLogo();
    }

    for (int i = 0; i < m; i++) {

        printf("\t\t\t\t       |       |       \n\t\t\t\t");    // print first empty line
        for (int j = 0; j < n; j++) {
            printf("   %c   ", board[i][j]); 
            if (j < n-1) {
                printf("|");                    // print separator 2 times each row
            }
        }
        printf("\n");
            if (i < m-1) {
                printf("\t\t\t\t_______|_______|_______\n");
            }
        if (i == m-1) {
            printf("\t\t\t\t       |       |       ");  // final print of empty line
        }
    }

    printf("\n\n");
}

void initializeBoard(int m, int n, char board[][n]) {
    int c = 1;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            board[i][j] = c + '0';
            c++;
        }
    }
}

//############################################################################################
//
//  Core functions from ASSIGNMENT 1   - END
//
//############################################################################################
