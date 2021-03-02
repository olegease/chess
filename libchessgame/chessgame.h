#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef enum ease_chessgame_Location {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
} ease_chessgame_Location;

typedef enum ease_chessgame_Color {
    Black, White
} ease_chessgame_Color;

typedef enum ease_chessgame_Piece {
    Null,
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRock, WhiteQueen, WhiteKing,
    BlackPawn, BlackKnight, BlackBishop, BlackRock, BlackQueen, BlackKing
} ease_chessgame_Piece;

typedef struct ease_chessgame_Fen {
    ease_chessgame_Piece board[64];
    ease_chessgame_Color player;
    _Bool kcastling[2];
    _Bool qcastling[2];
    ease_chessgame_Location enpassant;
    int halfclock;
    int fullmoves;
} ease_chessgame_Fen;

typedef struct ease_chessgame_Move {
    ease_chessgame_Location from;
    ease_chessgame_Location to;
} ease_chessgame_Move;

//void ease_chessgame_register();
//void ease_chessgame_unregister();
//void ease_chessgame_dirty_moves();
//void ease_chessgame_valid_moves();
//void ease_chasegame_move();
ease_chessgame_Fen ease_chessgame_parse_fen(const char* fenstr);
#ifdef __cplusplus
} // namespace ease::chess::c { extern "C" {
#endif
