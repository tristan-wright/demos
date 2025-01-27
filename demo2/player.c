#include "player.h"

/**
 * Updates a player from input information.
 * @param game The game where the player exists.
 * @param input The input given to update the player.
 * @return True if the player has been successfully updated.
 */
bool update(Game* game, char* input) {
    char* values = &input[3];
    int* data = malloc(sizeof(int) * 5);

    if (strlen(input) < 12) {
        return false;
    }

    data[0] = atoi(strtok(values, ","));

    for (int i = 1; i <= 4; ++i) {
        char* value = strtok(NULL, ",");
        data[i] = atoi(value);
    }

    if (data[0] > game->board->numPlayers) {
        return false;
    }

    Player* player = get_player(game->board, data[0]);
    player->points += data[2];
    player->money += data[3];

    if (data[1] > game->board->numStops) {
        return false;
    }

    if (!add_card(player, data[4])) {
        return false;
    }
    player = move(get_stop(game->board, player, 0),
            &game->board->stops[data[1]], player);
    print_player(player, stderr);
    return true;
}

/**
 * Determines how to handle the messages received by stdin.
 * @param game The game information.
 * @param input The text received from stdin.
 * @return True if a communication error has not been received.
 */
bool handle_input(Game* game, char* input) {
    game->me = get_player(game->board, game->pid);
    if(strstr(input, "YT") != NULL && strlen(input) == 3) {
        play_move(game, game->me);
        return true;
    } else if (strstr(input, "EARLY") != NULL && strlen(input) == 6) {
        return false;
    } else if (strstr(input, "DONE") != NULL && strlen(input) == 5) {
        game->alive = false;
        output_scores(game->board, stderr);
        return true;
    } else if (strstr(input, "HAP") != NULL) {
        if (update(game, input)) {
            print_board(game->board, stderr);
            return true;
        } else {
            return false;
        }

    }
    return false;
}

/**
 * Listens for game updates and responds with moves.
 * @param game The game information.
 * @return The exist status.
 */
int play(Game* game) {
    while(game->alive) {
        char* read = malloc(sizeof(char) * 80);

        if(feof(stdin) || fgets(read, 80, stdin) == NULL) {
            fprintf(stderr, "Communications error\n");
            return 6;
        } else if (!handle_input(game, read)) {
            fprintf(stderr, "Communications error\n");
            return 6;
        }
    }
    return 0;
}

/**
 * Initialises the board information.
 * @param game The game information.
 * @return The exit status.
 */
int initialise(Game* game) {
    fprintf(stdout, "^");
    fflush(stdout);

    if(!read_path(game->board, stdin)) {
        fprintf(stderr, "Invalid path\n");
        return 4;
    }

    print_board(game->board, stderr);

    return play(game);
}

/**
 * Checks that the staring arguments are correct.
 * @param argc The number of arguments.
 * @param argv The argument values.
 * @return The exit status.
 */
int main(int argc, char** argv) {
    Game* game = malloc(sizeof(Game));
    Board* board = malloc(sizeof(Board));
    game->alive = true;

    // Incorrect number of input parameters.
    if (argc != 3) {
        fprintf(stderr, "Usage: player pcount ID\n");
        return 1;
    }

    // Incorrect number of players.
    board->numPlayers = atoi(argv[1]);
    if (board->numPlayers < 1) {
        fprintf(stderr, "Invalid player count\n");
        return 2;
    }

    // Incorrect process id.
    game->pid = atoi(argv[2]);
    if (isdigit(*argv[2]) == 0 || game->pid < 0
            || game->pid >= board->numPlayers) {
        fprintf(stderr, "Invalid ID\n");
        return 3;
    }

    game->board = board;
    return initialise(game);
}
