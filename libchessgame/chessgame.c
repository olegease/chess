#include "chessgame.private.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __cplusplus

#endif

static Game* games[1];

// public section definitions
Id ease_chessgame_register(Fen fen)
{
    static bool is_first_call = true;
    if (is_first_call) {
        games[0] = NULL;
        is_first_call = false;
    }
    //if (games[0] == NULL) {
        games[0] = malloc(sizeof(Game));
        if (games[0]) {
            games[0]->fen = fen;
            games[0]->moves.elements = 0;
            games[0]->moves.capacity = 16;
            games[0]->moves.dirty = malloc(sizeof(*(games[0]->moves.dirty)) * games[0]->moves.capacity);
            if (!games[0]->moves.dirty) goto zero;
            games[0]->moves.valid = malloc(sizeof(*(games[0]->moves.valid)) * games[0]->moves.capacity);
            if (!games[0]->moves.valid) goto zero;
            return 1;
        }
    //}
zero:
    if (games[0]) {
        if (games[0]->moves.dirty) free(games[0]->moves.dirty);
        free(games[0]);
    }
    return 0;
}
void ease_chessgame_unregister(Id gameid)
{
    Id id = correct_id(gameid);
    free(games[id]->moves.dirty);
    free(games[id]->moves.valid);
    free(games[id]);
    games[correct_id(gameid)] = NULL;
}

bool ease_chessgame_is_registered(Id gameid)
{
    if (!gameid) return false;
    if (games[correct_id(gameid)] == NULL) return false;
    return true;
}

char ease_chessgame_piece_from_index(Id gameid, int index)
{
    Game* g = games[correct_id(gameid)];
    return g->fen.board[index];
}

Fen ease_chessgame_default_fen()
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

Fen ease_chessgame_parse_fen(const char* fenstr)
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

// private section definitions
Id correct_id(Id id)
{
    return id - 1;
}

Game* get_game(Id id)
{
    return games[correct_id(id)];
}

void Moves_manage(Moves* moves, Move move)
{
    if (moves->elements + 1 > moves->capacity) {
        int new_cap = moves->capacity * 2;
        Move* new_dirty = realloc(moves->dirty, new_cap);
        if (new_dirty) moves->dirty = new_dirty;
        int* new_valid = realloc(moves->valid, new_cap);
        if (new_valid) moves->valid = new_valid;
        moves->capacity = new_cap;
    }
    moves->dirty[moves->elements] = move;
    moves->elements++;
}

#ifdef __cplusplus

#endif
