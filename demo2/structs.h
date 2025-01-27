#ifndef ASS3_STRUCTS_H
#define ASS3_STRUCTS_H

#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <unistd.h>

struct Board;
struct Player;
struct Stop;
struct Game;

typedef struct PlayerType {
    pid_t pid;
    int money;
    int points;
    int visited[2];
    int* cards;
    int numCards;
    int curStop;
} Player;

typedef struct StopType {
    char type[2];
    int cap;
    bool isBarrier;
    int numPlaced;
    Player* placed;
} Stop;

typedef struct BoardType {
    int numPlayers;
    int numStops;
    Stop* stops;
    int numCards;
    int* cards;
} Board;

typedef struct GameType {
    Board* board;
    pid_t pid;
    Player* me;
    bool alive;
} Game;

#endif //ASS3_STRUCTS_H
