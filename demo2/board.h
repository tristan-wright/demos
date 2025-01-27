#ifndef ASS3_BOARD_H
#define ASS3_BOARD_H

#include "structs.h"

bool add_card(Player* player, int card);

bool read_path(Board* board, FILE* file);

Player* move(Stop* from, Stop* to, Player* player);

void print_board(Board* board, FILE* output);

void print_player(Player* player, FILE* output);

Player* get_player(Board* board, pid_t pid);

Stop* get_stop(Board* board, Player* player, int pos);

int position(Board* board, int start, char* type);

int distance(Board* board, Player* player, char* type);

void chosen_move(int position, FILE* output);

bool at_capacity(Stop* stop);

void output_scores(Board* board, FILE* output);

int player_pos(Board* board, Player* player);

bool valid_money_stop(Stop* stop);

bool valid_v2_stop(Stop* stop);

bool valid_ri_stop(Stop* stop);

#endif //ASS3_BOARD_H
