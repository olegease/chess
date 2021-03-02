#pragma once
#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#include <stdint.h>
#endif

const int ease_chessgame_BOARD_SIZE = 64;
const int ease_chessgame_BOARD_DIMENSION = 8;
const char ease_chessgame_PLAYER_WHITE = 'w';
const char ease_chessgame_PLAYER_BLACK = 'b';

enum {
    ease_chessgame_FLAG_WHITE_KSIDE_CASTLING,
    ease_chessgame_FLAG_WHITE_QSIDE_CASTLING,
    ease_chessgame_FLAG_BLACK_KSIDE_CASTLING,
    ease_chessgame_FLAG_BLACK_QSIDE_CASTLING
};

typedef struct {
    char board[64];
    int16_t fullmoves;
    char player;
    int8_t enpassant;
    int8_t halfclock;
    bool flags[8];
} ease_chessgame_Fen;

typedef struct {
    int from;
    int to;
} ease_chessgame_Move;

//void ease_chessgame_register();
//void ease_chessgame_unregister();
//void ease_chessgame_dirty_moves();
//void ease_chessgame_valid_moves();
//void ease_chasegame_move();
ease_chessgame_Fen ease_chessgame_default_fen();
ease_chessgame_Fen ease_chessgame_parse_fen(const char* fenstr);
#ifdef __cplusplus
} // { extern "C" {
#endif
