#include "chessgame.h"
#include "stdbool.h"

#ifdef __cplusplus

#endif

ease_chessgame_Fen ease_chessgame_default_fen()
{
    static const char board[] = "rnbqkbnrpppppppp00000000000000000000000000000000PPPPPPPPRNBQKBNR";
    ease_chessgame_Fen fen;
    for (int i = 0; i < ease_chessgame_BOARD_SIZE; ++i) fen.board[i] = board[i];
    fen.player = ease_chessgame_PLAYER_WHITE;
    fen.flags[ease_chessgame_FLAG_WHITE_KSIDE_CASTLING] = true;
    fen.flags[ease_chessgame_FLAG_WHITE_QSIDE_CASTLING] = true;
    fen.flags[ease_chessgame_FLAG_BLACK_KSIDE_CASTLING] = true;
    fen.flags[ease_chessgame_FLAG_BLACK_QSIDE_CASTLING] = true;
    fen.enpassant = ease_chessgame_BOARD_SIZE;
    fen.halfclock = 0;
    fen.fullmoves = 1;
    return fen;
}

ease_chessgame_Fen ease_chessgame_parse_fen(const char* fenstr)
{
    ease_chessgame_Fen fen;
    fen.board[0] = '0';
    return fen;
}

#ifdef __cplusplus

#endif
