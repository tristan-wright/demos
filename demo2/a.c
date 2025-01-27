#include "a.h"

bool search_money(Game* game, int curStop, Player* player);
bool search_v(Game* game, int curStop);
int player_pos(Board* board, Player* player);

/**
 * Determines the move for A type player.
 * @param game The game information to determine the move.
 * @param player The player to make the move for.
 */
void play_move(Game* game, Player* player) {
    int curStop = player_pos(game->board, player);
    // #1 case in user move choice.
    if (player->money > 0 && search_money(game, curStop, player)) {
        return;
    }

    // #2 case in user move choice.
    Stop* nextStop = get_stop(game->board, player, 0);
    nextStop++;
    if (valid_money_stop(nextStop)) {
        chosen_move(position(game->board, 0, "Mo"), stdout);
        return;
    }

    // #3 case in user move choice.
    if (search_v(game, curStop)) {
        return;
    }
}

/**
 * Searches for a money conversion stop to move the player to.
 * @param game The game information.
 * @param curStop The current stop of the player.
 * @param player The player to search for.
 * @return True if the player has chosen this stop.
 */
bool search_money(Game* game, int curStop, Player* player) {
    for (int i = curStop; i < game->board->numStops; ++i) {
        Stop* stop = &game->board->stops[i];
        if (strcmp(stop->type, "Do") == 0) {
            if (at_capacity(stop)) {
                return false;
            }
            chosen_move(position(game->board, curStop, "Do"), stdout);
            return true;
        }
    }
    return false;
}

/**
 * Searches for the next available V1, V2 or Barrier to move the player to.
 * @param game The game to search.
 * @param curStop The current stop of the player.
 * @return True if a suitable stop has been chosen.
 */
bool search_v(Game* game, int curStop) {
    for (int i = curStop + 1; i < game->board->numStops; ++i) {
        Stop* stop = &game->board->stops[i];
        if (strcmp(stop->type, "V1") == 0 && !at_capacity(stop)) {
            chosen_move(position(game->board, curStop + 1, "V1"), stdout);
            return true;
        } else if (strcmp(stop->type, "V2") == 0 && !at_capacity(stop)) {
            chosen_move(position(game->board, curStop + 1, "V2"), stdout);
            return true;
        } else if (strcmp(stop->type, "::") == 0 && !at_capacity(stop)) {
            chosen_move(position(game->board, curStop + 1, "::"), stdout);
            return true;
        }
    }
    return false;
}