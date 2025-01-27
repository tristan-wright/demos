#include "b.h"

bool others_later(Board* board, Player* player, int curStop);
void first_open(Board* board, int curStop);
bool look_for(Board* board, int curStop, bool (*check)(Stop*));
int next_barrier(Board* board, int curStop);
bool highest_card(Board* board, Player* player, int curStop);

/**
 * Determines the move for a B type player.
 * @param game The game information.
 * @param player The player to determine the move for.
 */
void play_move(Game* game, Player* player) {
    int curStop = player_pos(game->board, player);

    // #1 case in user move choice.
    if (others_later(game->board, player, curStop)) {
        //printf("A\n");
        return;
    }
    // #2 case in user move choice.
    if (player->money % 2 != 0 &&
            look_for(game->board, curStop, &valid_money_stop)) {
        //printf(" %d", player->money % 2);
        //printf("B\n");
        return;
    }

    // #3 case in user move choice.
    if (highest_card(game->board, player, curStop)) {
        //printf("C\n");
        return;
    }

    // #4 case in user move choice.
    if (look_for(game->board, curStop, &valid_v2_stop)) {
        //printf("D\n");
        return;
    }

    // #5 case in user move choice.
    first_open(game->board, curStop);
    return;
}

/**
 * Determines if all other players are ahead.
 * @param board The board to search for other players.
 * @param player The player to move for.
 * @param curStop The current stop of the player to move.
 * @return True if all other players are later and the
 *          player has been moved.
 */
bool others_later(Board* board, Player* player, int curStop) {
    if (get_stop(board, player, curStop)->numPlaced > 1) {
        return false;
    }

    for (int i = 0; i < curStop; ++i) {
        Stop* stop = &board->stops[i];
        if (stop->numPlaced > 0) {
            return false;
        }
    }

    if (at_capacity(&board->stops[curStop + 1])) {
        return false;
    }
    chosen_move(curStop + 1, stdout);
    return true;
}

/**
 * A function that looks for a stop to move the player from
 * input criteria.
 * @param board The board to search.
 * @param curStop The current stop of the player.
 * @param check The function pointer to the check criteria.
 * @return True if the player has been moved.
 */
bool look_for(Board* board, int curStop, bool (*check)(Stop*)) {
    int nextBarr = next_barrier(board, curStop);
    for (int i = curStop + 1; i < nextBarr; ++i) {
        Stop* stop = &board->stops[i];
        if (check(stop)) {
            chosen_move(i, stdout);
            return true;
        }
    }
    return false;
}

/**
 * Checks for the first open spot and move the player.
 * @param board The board to search for the next open stop.
 * @param curStop The current stop of the player.
 */
void first_open(Board* board, int curStop) {
    for (int i = curStop + 1; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        if (!at_capacity(stop)) {
            chosen_move(i, stdout);
            return;
        }
    }
}

/**
 * Searches for the next barrier.
 * @param board The board to search.
 * @param curStop The current spot to start the search.
 * @return The position of the next barrier on the board.
 */
int next_barrier(Board* board, int curStop) {
    for (int i = curStop + 1; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        if (strcmp(stop->type, "::") == 0) {
            return i;
        }
    }
    return 0;
}

/**
 * Checks if player has the highest card and moved them.
 * @param board The board to search.
 * @param player The player to move for.
 * @param curStop The current stop of the player.
 * @return True if the player has the highest number of cards
 *          and has been moved.
 */
bool highest_card(Board* board, Player* player, int curStop) {
    for (int i = 0; i < board->numStops; ++i) {
        Stop* stop = &board->stops[i];
        for (int j = 0; j < stop->numPlaced; ++j) {
            if (stop->placed[j].numCards > player->numCards) {
                return false;
            }
        }
    }

    if (look_for(board, curStop, &valid_ri_stop)) {
        return true;
    }
    return false;
}