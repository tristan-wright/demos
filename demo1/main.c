#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Game {
    char player1;
    char player2;
    char* saveFileName;
    int* rows;
    int* cols;
    char curTurn;
    char** board;
    int score[2];
    int oneRow;
    int oneCol;
};

/**
 * Determines whether a player type is correct.
 * @param playerType The player type entered.
 * @return True if a valid player type. Otherwise false.
 */
bool actual_player(char* playerType) {
    switch(*playerType) {
        case '0':
            return true;
        case '1':
            return true;
        case 'H':
            return true;
        default:
            return false;
    }
}

/**
 * Validates the characters found in the board.
 * @param boardChar The char in the board.
 * @return True if it's a valid character.
 */
bool valid_board_char(char boardTile) {
    if (isdigit(boardTile)) {
        return true;
    }

    switch (boardTile) {
        case '0':
            return true;
        case '.':
            return true;
        case ' ':
            return true;
        case 'X':
            return true;
        case 'O':
            return true;
        case '\n':
            return true;
        default:
            return false;
    }
}

/**
 * Prints out the contents of the board.
 * @param game The game information.
 * @param output The file pointer used to output to.
 */
void print_board(struct Game* game, FILE* output) {
    for (int k = 0; k < *game->rows; ++k) {
        for (int i = 0; i < *game->cols * 2; ++i) {
            fprintf(output, "%c", game->board[k][i]);
        }
        fprintf(output, "\n");
    }
    fflush(output);
    return;
}

/**
 * Loads the dimensions for the board.
 * @param game Game information.
 * @param line Dimension information string.
 * @return Zero if loaded successfully.
 */
int load_size(struct Game* game, char* line) {
    game->rows = malloc(sizeof(int));
    game->cols = malloc(sizeof(int));

    int rows = atoi(strtok(line, " "));
    *game->rows = rows;

    int cols = atoi(strtok(NULL, " "));
    *game->cols = cols;

    if (rows < 3 || cols < 3) {
        return 1;
    }
    return 0;
}

/**
 * Loads the current turn of the player.
 * @param game The game information.
 * @param line The current turn information string.
 * @return Zero if loaded successfully.
 */
int load_turn(struct Game* game, char* line) {
    if (!(line[0] == 'O' || line[0] == 'X')) {
        return 1;
    }
    game->curTurn = line[0];
    return 0;
}

/**
 * Loads the board into the game information.
 * @param game The game information.
 * @param buff A buffer to use when reading save.
 * @param file The file pointer to use when reading save file.
 * @return Zero if successful.
 */
int load_board(struct Game* game, char* buff, FILE* file) {
    char** board;

    board = malloc(sizeof(char*) * *game->rows);
    for (int l = 0; l < *game->rows; ++l) {
        board[l] = malloc(sizeof(char*) * *game->cols * 2);
    }

    for (int j = 0; j < *game->rows; ++j) {
        fgets(buff, 80, file);
        if (strcmp(buff, "\n") == 0) {
            return 1;
        }

        for (int i = 0; i < *game->cols * 2; ++i) {
            if (!valid_board_char(buff[i])) {
                return 1;
            }
            board[j][i] = buff[i];
        }
    }

    game->board = board;
    return 0;
}

/**
 * Gets any playable tile on the board.
 * @param game Game information.
 * @param row Row value for the move.
 * @param col Column value for the move.
 * @return The value in that tile.
 */
char* get_pos(struct Game* game, int row, int col) {
    return &game->board[row][(col * 2) + 1];
}

/**
 * Gets the value at a position in the board.
 * @param game Game information.
 * @param row Row value for score.
 * @param col Column value for score.
 * @return The value associated to that tile.
 */
int get_value(struct Game* game, int row, int col) {
    char value = game->board[row][(col * 2)];
    return atoi(&value);
}

/**
 * Checks to see if the board is full.
 * @param game The game information.
 * @return Returns true if the board is full.
 */
bool full_board(struct Game* game) {
    for (int row = 1; row < *game->rows - 1; ++row) {
        for (int col = 1; col < *game->cols - 1; ++col) {
            if (*get_pos(game, row, col) == '.') {
                return false;
            }
        }
    }
    return true;
}

/**
 * Handles the entire loading process for the game.
 * @param game Game information
 * @return Zero if successful.
 */
int load_save(struct Game* game) {
    FILE* file = fopen(game->saveFileName, "r");
    char* buff = malloc(sizeof(char) * 80);

    if (file == NULL) {
        return 3;
    }

    //Loads the board dimensions.
    fgets(buff, 80, file);
    if (load_size(game, buff)) {
        return 4;
    }

    //Loads the next player.
    fgets(buff, 80, file);
    if (load_turn(game, buff)) {
        return 4;
    }

    //Loads the board.
    if (load_board(game, buff, file)) {
        return 4;
    }

    if (full_board(game)) {
        return 6;
    }

    free(buff);
    fclose(file);
    return 0;
}

/**
 * Saves the board to the following filename.
 * @param game The current game information.
 * @param saveName The file to save to game to.
 * @return True if the save is successful.
 */
bool save_game(struct Game* game, char* saveName) {
    saveName[strlen(saveName) - 1] = '\0';

    FILE* file = fopen(saveName, "w");

    if (file == NULL) {
        return false;
    }

    fprintf(file, "%d %d\n", *game->rows, *game->cols);
    fprintf(file, "%c\n", game->curTurn);
    print_board(game, file);

    fclose(file);
    return true;
}

/**
 * Determines whether the current player is automated.
 * @param game Game information.
 * @return True if player is automated.
 */
bool is_auto_player(struct Game* game) {
    if (game->curTurn == 'O') {
        return (game->player1 != 'H');
    } else if (game->curTurn == 'X') {
        return (game->player2 != 'H');
    }
    return false;
}

/**
 * Updates the current turn in the game information.
 * @param game Game information.
 */
void next_turn(struct Game* game) {
    if (game->curTurn == 'O') {
        game->curTurn = 'X';
    } else if (game->curTurn == 'X') {
        game->curTurn = 'O';
    }
}

/**
 * Determines whether a move made is on the edge of the board.
 * @param game The game information.
 * @param row  The row position to play the move.
 * @param col  The column position to play the move.
 * @return True if it's on the edge of the board.
 */
bool is_edge(struct Game* game, int row, int col) {
    if (row == *game->rows - 1 || row == 0) {
        return true;
    } else if (col == *game->cols - 1 || col == 0) {
        return true;
    }
    return false;
}

/**
 * Determines if the move being played is in the position of
 * a corner.
 * @param game The game information.
 * @param row The row to check for the corner.
 * @param col The column to check for the corner.
 * @return True if the move is a corner.
 */
bool is_corner(struct Game* game, int row, int col) {
    if (row == 0 && col == 0) {
        return true;
    }
    if (row == 0 && col == *game->cols - 1) {
        return true;
    }
    if (col == 0 && row == *game->rows - 1) {
        return true;
    }
    if (row == *game->rows - 1 && col == *game->cols - 1) {
        return true;
    }
    return false;
}

/**
 * Determines whether a moe is valid.
 * @param game The game information.
 * @param row The row to check for the move.
 * @param col The column to check for the move.
 * @return True if the move is playable.
 */
bool valid_move(struct Game* game, int row, int col) {
    if (is_corner(game, row, col)) {
        return false;
    }

    return (*get_pos(game, row, col) == '.');
}

/**
 * Shifts the moves vertically.
 * @param game The game information.
 * @param row The row position to start the shift.
 * @param col The column position to start the shift.
 * @param reverse Direction in order of shift.
 * @return True if the shift is successful.
 */
bool handle_vert_shift(struct Game* game, int row, int col, bool reverse) {
    if (!valid_move(game, row, col)) {
        return false;
    }

    if (!reverse) {
        for (int i = *game->rows - 1; i > 0; --i) {
            if (i == 1) {
                *get_pos(game, i, col) = game->curTurn;
            } else {
                *get_pos(game, i, col) = *get_pos(game, i - 1, col);
            }
        }
    } else {
        for (int i = 0; i < *game->rows; ++i) {
            if (i == *game->rows - 1) {
                *get_pos(game, i - 1, col) = game->curTurn;
            } else {
                *get_pos(game, i, col) = *get_pos(game, i + 1, col);
            }
        }
    }
    return true;
}

/**
 * Shift the moves horizontally.
 * @param game The game information.
 * @param row The row position to start the shift.
 * @param col The column position to start the shift.
 * @param reverse Direction of the shift order.
 * @return True if the shift is successful.
 */
bool handle_hori_shift(struct Game* game, int row, int col, bool reverse) {
    if (!valid_move(game, row, col)) {
        return false;
    }

    if (!reverse) {
        for (int j = *game->cols - 1; j > 0; --j) {
            if (j == 1) {
                *get_pos(game, row, j) = game->curTurn;
            } else {
                *get_pos(game, row, j) = *get_pos(game, row, j - 1);
            }
        }
    } else {
        for (int j = 0; j < *game->cols; ++j) {
            if (j == *game->cols - 1) {
                *get_pos(game, row, j - 1) = game->curTurn;
            } else {
                *get_pos(game, row, j) = *get_pos(game, row, j + 1);
            }
        }
    }
    return true;
}

/**
 * Determines if the edge move made is valid.
 * @param game The game information.
 * @param row The row position on the edge.
 * @param col The column position on the edge.
 * @param type The type of edge that is being selected.
 * @return True if the move is a valid edge move.
 */
bool valid_edge_move(struct Game* game, int row, int col, char type) {
    switch (type) {
        case 'T':
            return (!valid_move(game, row + 1, col) &&
                    valid_move(game, *game->rows - 1, col));
        case 'R':
            return (!valid_move(game, row, col + 1) &&
                    valid_move(game, row, *game->cols - 1));
        case 'B':
            return (!valid_move(game, row - 1, col) &&
                    valid_move(game, 0, col));
        case 'L':
            return (!valid_move(game, row, col - 1) &&
                    valid_move(game, row, 0));
        default:
            return false;
    }
}

/**
 * Determines they handling for a move played on the outer
 * edge of the board and shifts accordingly.
 * @param game The game information.
 * @param row The row position of the outer move.
 * @param col The column position of the outer move.
 * @return True if the move is allowed and successfully executed.
 */
bool handle_edge_move(struct Game* game, int row, int col) {
    if (is_corner(game, row, col)) {
        return false;
    }
    // The board's top edge.
    if (row == 0 && valid_edge_move(game, row, col, 'T')) {
        return handle_vert_shift(game, row, col, false);

    // The board's left edge.
    } else if (col == 0 && valid_edge_move(game, row, col, 'R')) {
        return handle_hori_shift(game, row, col, false);

    // The board's bottom edge.
    } else if (row == *game->rows - 1 && 
            valid_edge_move(game, row, col, 'B')) {
        return handle_vert_shift(game, row, col, true);

    // The board's right edge.
    } else if (col == *game->cols - 1 && 
            valid_edge_move(game, row, col, 'L')) {
        return handle_hori_shift(game, row, col, true);
    }
    return false;
}

/**
 * Determines how the move will affect the board.
 * @param game The game information.
 * @param row The row to play the move.
 * @param col The column to play the move.
 * @return True if the move is played. Otherwise false.
 */
bool play_move(struct Game* game, int row, int col) {
    if (row > *game->rows || col > *game->cols) {
        return false;
    }

    if (is_edge(game, row, col)) {
        return handle_edge_move(game, row, col);
    }

    if (!valid_move(game, row, col)) {
        return false;
    }

    *get_pos(game, row, col) = game->curTurn;
    return true;
}

/**
 * Determines the score for each player in the game.
 * @param game Game information.
 */
void get_score(struct Game* game) {
    for (int row = 1; row < *game->rows - 1; ++row) {
        for (int col = 1; col < *game->cols - 1; ++col) {

            if (*get_pos(game, row, col) == 'O') {
                game->score[0] += get_value(game, row, col);
            } else {
                game->score[1] += get_value(game, row, col);
            }
        }
    }
}

/**
 * Handles the scoring system for the game.
 * @param game Game information.
 */
void handle_score(struct Game* game) {
    get_score(game);

    if (game->score[0] < game->score[1]) {
        fprintf(stdout, "Winners: X\n");
    } else if (game->score[0] > game->score[1]) {
        fprintf(stdout, "Winners: O\n");
    } else {
        fprintf(stdout, "Winners: O X\n");
    }
    fflush(stdout);
}

/**
 * Handles the move given by the player.
 * @param game The game information.
 * @param buff The buffer containing move information.
 * @return True if the move was successful.
 */
bool handle_move(struct Game* game, char* buff) {
    char input[2][80];
    int count = 0;

    // Counts number of words.
    for (int j = 0; j < strlen(buff); ++j) {
        if (buff[j] == ' ') {
            count++;
        }
    }

    // Check input only has two words.
    if (count == 1) {
        for (int i = 0; i < 2; ++i) {
            if (i == 0) {
                strcpy(input[i], strtok(buff, " "));
            } else {
                strcpy(input[i], strtok(NULL, " "));
            }
        }
        return play_move(game, atoi(input[0]), atoi(input[1]));
    }
    return false;
}

/**
 * Handles the input given to the board to determine action.
 * @param game The game information.
 * @return True if the move didn't end game.
 */
bool handle_input(struct Game* game) {
    char* buff = malloc(sizeof(char) * 80);
    bool status = false;

    while (!status) {
        fprintf(stdout, "%c:(R C)> ", game->curTurn);
        fflush(stdout);
        fgets(buff, 80, stdin);

        if (feof(stdin)) {
            return false;
        }

        if (buff[0] == 's' && strlen(buff) > 2) {
            if (!save_game(game, &buff[1])) {
                fprintf(stderr, "Save failed\n");
                fflush(stderr);
            }
        }
        status = handle_move(game, buff);

    }
    return true;
}

/**
 * Prints out the current automated players move.
 * @param curTurn The current turn.
 * @param row Which row has been played.
 * @param col Which column has been played.
 */
void print_auto_move(char curTurn, int row, int col) {
    fprintf(stdout, "Player %c placed at %d %d\n", curTurn, row, col);
    fflush(stdout);
}

/**
 * Automated player logic for player type zero.
 * @param game Game information.
 * @param reverse Determines the order to start searching.
 */
void play_auto_zero(struct Game* game, bool reverse) {
    if (!reverse) {
        for (int i = 1; i < *game->rows - 1; ++i) {
            for (int j = 1; j < *game->cols - 1; ++j) {
                if (play_move(game, i, j)) {
                    print_auto_move(game->curTurn, i, j);
                    return;
                }
            }
        }
    } else {
        for (int i = *game->rows - 2; i > 0; --i) {
            for (int j = *game->cols - 2; j > 0; --j) {
                if (play_move(game, i, j)) {
                    print_auto_move(game->curTurn, i, j);
                    return;
                }
            }
        }
    }
}

/**
 * The default playing algorithm for the type 1
 * automated player.
 * @param game The game information.
 */
void play_highest(struct Game* game) {
    int highest = 0;
    int highrow;
    int highcol;

    for (int i = 0; i < *game->rows; ++i) {
        for (int j = 0; j < *game->cols; ++j) {
            int value = get_value(game, i, j);
            if (valid_move(game, i, j) && value > highest) {
                highest = value;
                highrow = i;
                highcol = j;
            }
        }
    }

    if (play_move(game, highrow, highcol)) {
        print_auto_move(game->curTurn, highrow, highcol);
        return;
    }
}

/**
 * Determines if the current player has played a move
 * in this position.
 * @param game The game information.
 * @param row The row information.
 * @param col The column information.
 * @return True if the player has placed in that position.
 */
bool my_position(struct Game* game, int row, int col) {
    return *get_pos(game, row, col) == game->curTurn;
}

/**
 * Determines the score in a column.
 * @param game The game information.
 * @param col The column to score.
 * @param offset The row offset for potential move scoring.
 * @return The score after the offset is applied.
 */
int column_score(struct Game* game, int col, int offset) {
    int score = 0;
    for (int row = 1; row < *game->rows - 1; ++row) {
        if (!valid_move(game, row, col) && !my_position(game, row, col)) {
            score += get_value(game, row + offset, col);
        }
    }
    return score;
}

/**
 * Determines the score in a row.
 * @param game The game information.
 * @param row The row to score.
 * @param offset The column offset for potential more scoring.
 * @return The score after the offset is applied.
 */
int row_score(struct Game* game, int row, int offset) {
    int score = 0;
    for (int col = 1; col < *game->cols - 1; ++col) {
        if (!valid_move(game, row, col) && !my_position(game, row, col)) {
            score += get_value(game, row, col + offset);
        }
    }
    return score;
}

/**
 * Playing the one type move.
 * @param game The game information.
 * @param score The current reduction score.
 */
void play_auto_one_move(struct Game* game, int score) {
    if (score < 0) {
        if (!play_move(game, game->oneRow, game->oneCol)) {
            play_highest(game);
        } else {
            print_auto_move(game->curTurn, game->oneRow, game->oneCol);
        }
    } else {
        play_highest(game);
    }
}

/**
 * The game logic for type zero automated player.
 * @param game The game information.
 */
void play_auto_one(struct Game* game) {
    int score = 0;
    int newScore;
    // Check for lowering score Top.
    for (int i = 1; i < *game->cols - 1; ++i) {
        newScore = column_score(game, i, 1) - column_score(game, i, 0);
        if (newScore < score && valid_edge_move(game, 0, i, 'T')) {
            score = newScore;
            game->oneRow = 0;
            game->oneCol = i;
            play_auto_one_move(game, score);
            return;
        }
    }
    // Checks for lowering score Right.
    for (int j = 1; j < *game->rows - 1; ++j) {
        newScore = row_score(game, j, -1) - row_score(game, j, 0);
        if (newScore < score && 
                valid_edge_move(game, j, *game->cols - 1, 'R')) {
            score = newScore;
            game->oneRow = j;
            game->oneCol = *game->cols - 1;
            play_auto_one_move(game, score);
            return;
        }
    }
    // Check for lowering score Bottom.
    for (int i = 1; i < *game->cols - 1; ++i) {
        newScore = column_score(game, i, -1) - column_score(game, i, 0);
        if (newScore < score && 
                valid_edge_move(game, *game->rows - 1, i, 'B')) {
            score = newScore;
            game->oneRow = *game->rows - 1;
            game->oneCol = i;
            play_auto_one_move(game, score);
            return;
        }
    }
    // Check for lowering score Left.
    for (int j = 1; j < *game->rows - 1; ++j) {
        newScore = row_score(game, j, 1) - row_score(game, j, 0);
        if (newScore < score && valid_edge_move(game, j, 0, 'L')) {
            score = newScore;
            game->oneRow = j;
            game->oneCol = 0;
            play_auto_one_move(game, score);
            return;
        }
    }
    play_auto_one_move(game, score);
}

/**
 * Determines which automated players turn is current.
 * @param game Game information.
 */
void handle_auto(struct Game* game) {
    if (game->curTurn == 'O') {
        if (game->player1 == '0') {
            play_auto_zero(game, false);
        } else {
            play_auto_one(game);
        }
    } else {
        if (game->player2 == '0') {
            play_auto_zero(game, true);
        } else {
            play_auto_one(game);
        }
    }
}

/**
 * Main game loop.
 * @param game Game information.
 * @return
 */
int main_game(struct Game* game) {
    bool active = true;
    print_board(game, stdout);

    while(active) {
        if (is_auto_player(game)) {
            handle_auto(game);
        } else {
            if (!(handle_input(game))) {
                fprintf(stderr, "End of file\n");
                fflush(stderr);
                return 5;
            }
        }
        print_board(game, stdout);
        if (full_board(game)) {
            handle_score(game);
            return 0;
        }
        next_turn(game);
    }
    return 0;
}

/**
 * Initial sequence to prepare the game before
 * the board becomes playable.
 * @param game The struct holding the game information
 * @return The exit status of the game.
 */
int start_game(struct Game* game) {
    int status = load_save(game);

    // Handles error codes relating to loading the game file.
    if (status != 0) {
        if (status == 3) {
            fprintf(stderr, "No file to load from\n");
        } else if (status == 4) {
            fprintf(stderr, "Invalid file contents\n");
        } else if (status == 6) {
            fprintf(stderr, "Full board in load\n");
        }
        fflush(stderr);
        return status;
    }
    return main_game(game);
}

/**
 * Start of the game and loading parameters.
 * @param argc The number of parameters.
 * @param argv The input parameters.
 * @return Zero if game executed successfully.
 */
int main(int argc, char** argv) {
    struct Game* game = malloc(sizeof(struct Game));
    game->saveFileName = argv[3];

    // Checks for valid number of payers.
    if (argc != 4) {
        fprintf(stderr, "Usage: push2310 typeO typeX fname\n");
        fflush(stderr);
        return 1;
    }

    // Checks for valid player.
    if (!actual_player(argv[1]) || !actual_player(argv[2])) {
        fprintf(stderr, "Invalid player type\n");
        fflush(stderr);
        return 2;
    } else {
        game->player1 = *argv[1];
        game->player2 = *argv[2];
    }

    return start_game(game);
}
