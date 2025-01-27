#ifndef ASS3_MAIN_H
#define ASS3_MAIN_H

#include "board.h"
#include "structs.h"

struct Hub;

typedef struct HubType {
    Board* board;
    char** players;
    int** fdreceive;
    int** fdsend;
    FILE** receive;
    FILE** send;
    char* path;
} Hub;

#endif //ASS3_MAIN_H
