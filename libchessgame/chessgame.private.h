#pragma once
#include "chessgame.h"
#ifdef __cplusplus
extern "C" {
#endif
// private section declarations
typedef ease_chessgame_Fen Fen;
typedef ease_chessgame_Move Move;
typedef ease_chessgame_ID Id;
typedef struct Moves Moves;
typedef struct Game Game;

struct Moves {
    Move* dirty;
    int* valid; // valid array of indexes to dirty moves array
    int elements; // currently occupied in dirty and then valid array after validation
    int capacity; // how much elements can store dirty and valid arrays
};

struct Game {
    Fen fen;
    Moves moves;
};

Game* get_game(Id id);
void Moves_manage(Moves* moves, Move move);
void Moves_dirty(Moves moves);
void Moves_valid(Moves moves);
Id correct_id(Id id);

#ifdef __cplusplus
}
#endif
