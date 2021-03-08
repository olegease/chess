#pragma once
#include "chessgame.h"
#ifdef __cplusplus
extern "C" {
#endif
// private section declarations
typedef ease_chessgame_Fen Fen;
typedef ease_chessgame_Move Move;
typedef ease_chessgame_ID Id;
typedef ease_chessgame_Piece Piece;
typedef ease_chessgame_Player Player;
typedef ease_chessgame_Index Index;
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

Game* Game_get(Id id);
Game* Game_create(Fen fen);
void Game_move(Game* game, Move move);
void Game_destroy(Game* game);
void Moves_manage(Moves* moves, Move move);
void Moves_dirty();
void Moves_valid(Moves moves);
Id correct_id(Id id);
void Piece_swap(Piece* l, Piece* r);

#ifdef __cplusplus
}
#endif
