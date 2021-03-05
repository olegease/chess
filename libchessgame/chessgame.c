#include "chessgame.h"
#include "stdbool.h"
#include "stdlib.h"
#include "ctype.h"

#ifdef __cplusplus

#endif

typedef ease_chessgame_Fen Fen;

typedef struct {
    Fen fen;
} Game;

Game* game[1];

ease_chessgame_ID correct_id(ease_chessgame_ID gameid) {
    return gameid - 1;
}

ease_chessgame_ID ease_chessgame_register(ease_chessgame_Fen fen)
{
    static bool is_first_call = true;
    if (is_first_call) {
        game[0] = NULL;
        is_first_call = false;
    }
    if (game[0] == NULL) {
        game[0] = malloc(sizeof(Game));
        if (game[0]) {
            game[0]->fen = fen;
            return 1;
        }
    }
    return 0;
}
void ease_chessgame_unregister(ease_chessgame_ID gameid)
{
    free(game[correct_id(gameid)]);
    game[correct_id(gameid)] = NULL;
}

bool ease_chessgame_is_registered(ease_chessgame_ID gameid)
{
    if (!gameid) return false;
    if (game[correct_id(gameid)] == NULL) return false;
    return true;
}

char ease_chessgame_piece_from_index(ease_chessgame_ID gameid, int index)
{
    Game* g = game[correct_id(gameid)];
    return g->fen.board[index];
}

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
    int curindex = 0;
    while (!isspace(*fenstr)) {
        if (*fenstr == '/') {
            
        }
        else if (isdigit(*fenstr)) {
            int count = atoi(fenstr);
            while (count--) fen.board[curindex++] = '0';
        }
        else {
            fen.board[curindex++] = *fenstr;
        }
        fenstr++;
    }
    fenstr++;
    fen.player = *fenstr;
    fenstr += 2;
    bool castlingflags[4] = {false, false, false, false};
    if (*fenstr == '-') {
        fenstr++;
    } else {
        while (!isspace(*fenstr)) {
            switch (*fenstr) {
            case 'K': castlingflags[ease_chessgame_FLAG_WHITE_KSIDE_CASTLING] = true; break;
            case 'Q': castlingflags[ease_chessgame_FLAG_WHITE_QSIDE_CASTLING] = true; break;
            case 'k': castlingflags[ease_chessgame_FLAG_BLACK_KSIDE_CASTLING] = true; break;
            case 'q': castlingflags[ease_chessgame_FLAG_BLACK_QSIDE_CASTLING] = true; break;
            }
            fenstr++;
        }
    }
    for (int i = 0; i < 4; i++) fen.flags[i] = castlingflags[i];
    fenstr++;
    if (*fenstr == '-') {
        fen.enpassant = ease_chessgame_BOARD_SIZE;
    }
    else {
        fen.enpassant = (int8_t)ease_chessgame_index_from_location(fenstr);
        fenstr++;
    }
    fenstr++;
    char* tail;
    while (!isspace(*fenstr++));
    fen.halfclock = (int8_t)strtol(fenstr, &tail, 10);
    fen.fullmoves = (int16_t)strtol(tail, &tail, 10);
    return fen;
}

int ease_chessgame_index_from_location(const char* location)
{
    return ease_chessgame_BOARD_DIMENSION * (ease_chessgame_BOARD_DIMENSION - (location[1] - '0')) + (location[0] - 'a');
}

#ifdef __cplusplus

#endif
