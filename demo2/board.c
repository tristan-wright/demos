#include "board.h"

int contains(Stop* stop, pid_t pid);
bool add_card(Player* player, int card);
int count_cards(Player* player, int card);
int lowest_card(int* cards);
int value_cards(Player* player);
bool add_player(Stop* stop, Player* player);
bool remove_player(Stop* stop, Player* player);
Player* move(Stop* from, Stop* to, Player* player);
int highest_placed(Board* board);
bool add_stops(Board* board, char* path);
void add_players(Board* board);
bool read_path(Board* board, FILE* file);
void print_board(Board* board, FILE* output);
void print_player(Player* player, FILE* output);
Player* get_player(Board* board, pid_t pid);
Stop* get_stop(Board* board, Player* player, int pos);
int position(Board* board, int start, char* type);
int distance(Board* board, Player* player, char* type);
void chosen_move(int distance, FILE* output);
bool at_capacity(Stop* stop);
int score(Player* player);
void output_scores(Board* board, FILE* output);
int player_pos(Board* board, Player* player);
bool valid_money_stop(Stop* stop);
bool valid_v2_stop(Stop* stop);
bool valid_ri_stop(Stop* stop);

/**
 * Determines whether a player is placed at a certain stop.
 * @param stop The Stop struct to search for player.
 * @param pid The player id check at the given stop.
 * @return The position of the player at the stop.
 */
int contains(Stop* stop, pid_t pid) {
    for (int i = 0; i < stop->numPlaced; ++i) {
        if (stop->placed[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

/**
 * Adds a card to a player's card inventory.
 * @param player The Player struct to add the card to.
 * @param card The Int value of the card to add.
 * @return
 */
bool add_card(Player* player, int card) {
    if (card == 0) {
        return true;
    } else if (card > 5) {
        return false;
    }
    player->cards = realloc(player->cards, sizeof(int)
            * (player->numCards + 1));
    player->cards[player->numCards] = card;
    player->numCards++;
    return true;
}

/**
 * Counts the number of card types that a player has in their card inventory.
 * @param player The Player struct to search for cards.
 * @param card The Int value of the card to search for.
 * @return The number of that type of cards the player has.
 */
int count_cards(Player* player, int card) {
    int counter = 0;
    for (int i = 0; i < player->numCards; ++i) {
        if (player->cards[i] == card) {
            counter++;
        }
    }
    return counter;
}

/**
 * Searches for the lowest number of a single type of card.
 * @param cards The type of card to search for.
 * @return The position of the lowest type of card.
 */
int lowest_card(int* cards) {
    int low = INT_MAX;
    int pos = 5;

    for (int i = 0; i < 5; ++i) {
        if (cards[i] < low && cards[i] > 0) {
            low = cards[i];
            pos = i;
        }
    }
    return pos;
}

/**
 * Calculates the point value of all the cards a player has in their
 * card inventory.
 * @param player The player struct information to search for cards.
 * @return The value of all the cards that the player has.
 */
int value_cards(Player* player) {
    int cardVals[5] = {1, 3, 5, 7, 10};
    int cards[5];
    int score = 0;

    for (int i = 0; i < 5; ++i) {
        cards[i] = count_cards(player, i + 1);
    }

    while (true) {
        int lowCount = lowest_card(cards);
        if (lowCount == 5) {
            break;
        }

        int count = 0;
        for (int i = 0; i < 5; ++i) {
            if (cards[i] > 0) {
                cards[i]--;
                count++;
            }
        }
        score += cardVals[count - 1];
    }
    return score;
}

/**
 * Adds a player to a stop.
 * @param stop The Stop struct to add the player to.
 * @param player The Player to be added to the struct.
 * @return True if the player was successfully added to the stop.
 */
bool add_player(Stop* stop, Player* player) {
    if (stop->cap == stop->numPlaced) {
        return false;
    }

    if (stop->numPlaced < 1) {
        stop->placed[0] = *player;
    } else {
        stop->placed[stop->numPlaced] = *player;
    }

    stop->numPlaced += 1;
    return true;
}

/**
 * Removes a Player from a given Stop.
 * @param stop The stop to remove the player from.
 * @param player The player to be removed.
 * @return True if the player was successfully removed from the stop.
 */
bool remove_player(Stop* stop, Player* player) {
    int pos = contains(stop, player->pid);

    if (pos >= 0) {
        for (int i = pos; i < stop->numPlaced - 1; ++i) {
            stop->placed[i] = stop->placed[i + 1];
        }
        stop->numPlaced--;
        return true;
    }

    return false;
}

/**
 * Moved a player from one stop to another.
 * @param from The stop that the player currently sits.
 * @param to The stop to move the player to.
 * @param player The player to be mvoed.
 * @return True if the player has been moved.
 */
Player* move(Stop* from, Stop* to, Player* player) {
    if (strcmp(to->type, "V1") == 0) {
        player->visited[0] += 1;
    } else if (strcmp(to->type, "V2") == 0) {
        player->visited[1] += 1;
    }
    return (add_player(to, player) &&
            remove_player(from, player)) ? player : NULL;
}

/**
 * Determines the high numbers of players placed at a
 * single stop on the board.
 * @param board The board information pointer.
 * @return The highest number of players at a single stop.
 */
int highest_placed(Board* board) {
    int high = 0;
    for (int i = 0; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        if (stop->numPlaced > high) {
            high = stop->numPlaced;
        }
    }
    return high;
}

/**
 * Adds and initialises stops for the board.
 * @param board The board struct information.
 * @param path The string of path information for stops.
 * @return True if all stops have been added successfully.
 */
bool add_stops(Board* board, char* path) {
    board->stops = malloc(sizeof(Stop) * board->numStops);
    for (int i = 0; i < board->numStops * 3; i += 3) {
        Stop* stop = malloc(sizeof(Stop));
        strncpy(stop->type, path + i, 2);
        char* cap = path + i + 2;

        if (isdigit(*cap)) {
            stop->cap = atoi(cap);
        } else if (*cap == '-') {
            stop->cap = board->numPlayers;
            stop->isBarrier = true;
        } else {
            return false;
        }

        stop->placed = malloc(sizeof(Player) * board->numPlayers);
        stop->numPlaced = 0;

        board->stops[i / 3] = *stop;
    }
    return true;
}

/**
 * Adds starting players to the board.
 * @param board The board to add the starting players to.
 */
void add_players(Board* board) {
    for (int j = board->numPlayers - 1; j >= 0; --j) {
        Player* newPlayer = malloc(sizeof(Player));
        newPlayer->pid = j;
        newPlayer->money = 7;
        newPlayer->points = 0;
        newPlayer->visited[0] = 0;
        newPlayer->visited[1] = 0;
        add_player(&board->stops[0], newPlayer);
    }
}

/**
 * Handles the reading and processing of the path information.
 * @param board The board information.
 * @param file The file pointer to read from.
 * @return True if successfully loaded.
 */
bool read_path(Board* board, FILE* file) {
    char* buff = calloc(256, sizeof(char));

    if(feof(stdin)) {
        return false;
    }

    fgets(buff, 255, file);
    char* numStops = strtok(buff, ";");
    char* path = strtok(NULL, ";");
    if (numStops == NULL || path == NULL) {
        return false;
    }

    board->numStops = atoi(numStops);
    if (board->numStops < 2) {
        return false;
    }

    int length = strlen(path);
    if (length != board->numStops * 3 + 1) {
        return false;
    }

    if (!add_stops(board, path)) {
        return false;
    }

    if (strcmp(board->stops[0].type, "::") != 0 ||
            strcmp(board->stops[board->numStops - 1].type, "::") != 0) {
        return false;
    }

    add_players(board);
    return true;
}

/**
 * Prints the current status of the board.
 * @param board The board information.
 * @param file The file stream to output to.
 */
void print_board(Board* board, FILE* output) {
    for (int j = 0; j < board->numStops; ++j) {
        Stop* stop = &board->stops[j];
        fprintf(output, "%s ", stop->type);
    }
    fprintf(output, "\n");

    int mostPlaced = highest_placed(board);

    char** location = calloc(board->numPlayers, sizeof(char*));
    for (int i = 0; i < board->numPlayers; ++i) {
        location[i] = calloc(board->numStops * 3, sizeof(char));
        memset(location[i], ' ', board->numStops * 3 - 1);
    }

    for (int k = 0; k < board->numStops; ++k) {
        Stop* stop = &board->stops[k];
        for (int i = 0; i < stop->numPlaced; ++i) {
            location[i][k * 3] = stop->placed[i].pid + '0';
        }
    }

    for (int l = 0; l < mostPlaced; ++l) {
        fprintf(output, "%s \n", location[l]);
    }
}

/**
 * Prints the information of the player that moved.
 * @param player The player who just moved.
 * @param output The file stream to output the information to.
 */
void print_player(Player* player, FILE* output) {
    int cards[5];
    for (int i = 1; i < 6; ++i) {
        cards[i - 1] = count_cards(player, i);
    }

    fprintf(output, "Player %d Money=%d V1=%d V2=%d "
            "Points=%d A=%d B=%d C=%d D=%d E=%d\n",
            player->pid, player->money, player->visited[0], player->visited[1],
            player->points, cards[0], cards[1], cards[2], cards[3], cards[4]);
}

/**
 * Get's the player information for a player from the board.
 * @param board The board to get the player info from.
 * @param pid The id of the player to get the location.
 * @return The pointer to the location of the Player.
 */
Player* get_player(Board* board, pid_t pid) {
    for (int i = 0; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        for (int j = 0; j < stop->numPlaced; ++j) {
            if (stop->placed[j].pid == pid) {
                return &stop->placed[j];
            }
        }
    }
    return NULL;
}

/**
 * Gets the Stop that a player is positioned on in the board.
 * @param board The board to search for the player.
 * @param player The player information to search for.
 * @param pos The starting position of the search on the board.
 * @return The location of the stop information on the board.
 */
Stop* get_stop(Board* board, Player* player, int pos) {
    for (int i = pos; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        for (int j = 0; j < stop->numPlaced; ++j) {
            if (stop->placed[j].pid == player->pid) {
                return stop;
            }
        }
    }
    return NULL;
}

/**
 * The position of a type of stop on the board.
 * @param board THe board to search for the position of the stop.
 * @param start The start position of the search.
 * @param type The type of stop to search for.
 * @return The location of the stop on the board.
 */
int position(Board* board, int start, char* type) {
    for (int i = start; i < board->numStops; ++i) {
        if (strcmp(board->stops[i].type, type) == 0) {
            return i;
        }
    }
    return 0;
}

/**
 * Gets the distance between a type of stop and a player.
 * @param board The board to search.
 * @param player The player to search for.
 * @param type The type of stop to get the distance.
 * @return The distance from the player to the given stop.
 */
int distance(Board* board, Player* player, char* type) {
    return position(board, 0, type) -
            position(board, 0, get_stop(board, player, 0)->type);
}

/**
 * Outputs the chosen move by the player.
 * @param distance The position of the stop to move to.
 * @param output The file stream to output the message to.
 */
void chosen_move(int distance, FILE* output) {
    fprintf(output, "DO%d\n", distance);
}

/**
 * Determines if a stop is at it's full capacity.
 * @param stop The stop to check the capacity.
 * @return True if the stop is full.
 */
bool at_capacity(Stop* stop) {
    return (stop->numPlaced == stop->cap) ? true : false;
}

/**
 * Gets the total score of a player.
 * @param player The player to determine the score.
 * @return The final score for the player.
 */
int score(Player* player) {
    int score = 0;
    score += player->visited[0];
    score += player->visited[1];
    score += player->points;
    score += value_cards(player);
    return score;
}

/**
 * Outputs the final scores for all the players.
 * @param board The board that contains the players.
 * @param output The file stream to output the score to.
 */
void output_scores(Board* board, FILE* output) {
    int* scores = calloc(board->numPlayers, sizeof(int));
    char* line = calloc(256, sizeof(char));
    char buffer[256];
    strcat(line, "Scores: ");

    for (int i = 0; i < board->numPlayers; ++i) {
        Player* player = get_player(board, i);
        scores[i] = score(player);
        sprintf(buffer, "%d", scores[i]);
        strcat(line, buffer);
        if (i < board->numPlayers - 1) {
            strcat(line, ",");
        }
    }
    fprintf(output, "%s\n", line);
}

/**
 * Gets a players position on the board.
 * @param board The board to search for the player.
 * @param player The player to search for.
 * @return The position of the player on the board.
 */
int player_pos(Board* board, Player* player) {
    for (int i = 0; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        for (int j = 0; j < stop->numPlaced; ++j) {
            if (stop->placed[j].pid == player->pid) {
                return i;
            }
        }
    }
    return 0;
}

/**
 * Check used to determine if a stop is a money stop.
 * @param stop The stop to check.
 * @return True if it is a money stop and space is available.
 */
bool valid_money_stop(Stop* stop) {
    return strcmp(stop->type, "Mo") == 0 && !at_capacity(stop);
}

/**
 * Check used to determine if a stop is a V2 type stop.
 * @param stop The stop to check.
 * @return True if it is a V2 stop and space is available.
 */
bool valid_v2_stop(Stop* stop) {
    return strcmp(stop->type, "V2") == 0 && !at_capacity(stop);
}

/**
 * Checks used to determine if a stop is a R1 stop.
 * @param stop The stop to check.
 * @return True if it is a R1 stop and space is available.
 */
bool valid_ri_stop(Stop* stop) {
    return strcmp(stop->type, "Ri") == 0 && !at_capacity(stop);
}