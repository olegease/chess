#include "chessgame.private.h"
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

    games[0] = Game_create(fen);
    if (games[0]) return 1;
    return 0;
}
void ease_chessgame_unregister(Id gameid)
{
    Game_destroy(Game_get(gameid));
    games[correct_id(gameid)] = NULL;
}

bool ease_chessgame_is_registered(Id gameid)
{
    if (!gameid) return false;
    if (!games[correct_id(gameid)]) return false;
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

void ease_chessgame_move(Id gameid, Move move)
{
    Game* game = Game_get(gameid);
    Game_move(game, move);
}

// private section definitions
Id correct_id(Id id)
{
    return id - 1;
}

int row_from_index(Index index)
{
    return index / ease_chessgame_BOARD_DIMENSION + 1;
}
int col_from_index(Index index)
{
    return index % ease_chessgame_BOARD_DIMENSION + 1;
}

void Piece_swap(Piece* lhs, Piece* rhs)
{
    Piece tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

Game* Game_create(Fen fen)
{
    Game* created = malloc(sizeof(Game));
    if (created) {
        created->fen = fen;
        created->moves.elements = 0;
        created->moves.capacity = 16;
        created->moves.dirty = malloc(sizeof *(created->moves.dirty) * created->moves.capacity);
        if (!created->moves.dirty) {
            free(created);
            return NULL;
        }
        created->moves.valid = malloc(sizeof *(created->moves.valid) * created->moves.capacity);
        if (!created->moves.valid) {
            free(created->moves.dirty);
            free(created);
            return NULL;
        }

    }

    return created;
}

Game* Game_get(Id id)
{
    return games[correct_id(id)];
}

void Game_move(Game* game, Move move)
{
    Piece* p = &(game->fen.board[move.from]);
    Piece* o = &(game->fen.board[move.to]);
    *o = '0';
    Piece_swap(p, o);
}

void Game_destroy(Game* game)
{
    free(game->moves.dirty);
    free(game->moves.valid);
    free(game);
}

void Game_moves_dirty(Game* game)
{
    Moves* moves = &game->moves;
    int white = game->fen.player == ease_chessgame_PLAYER_WHITE ? 1 : 0;
    Index doublemove_row[2] = { 2, 7 };
    int dir = white ? -1 : 1;
    for (int i = 0; i < ease_chessgame_BOARD_SIZE; ++i) {
        Piece piece = game->fen.board[i];
        if (piece == '0') continue;
        int opponent = !white;
        if (white) {
            if (islower(piece)) continue;
        }
        else {
            if (isupper(piece)) continue;
        }
        Move move = {i, ease_chessgame_BOARD_SIZE};
        switch (toupper(piece)) {
        case 'P': {
            move.to = move.from + 8 * dir;
            if (game->fen.board[move.to] == '0') {
                Moves_manage(moves, move);
                move.to = move.from + 16 * dir;
                if (game->fen.board[move.to] == '0' && row_from_index(move.from) == doublemove_row[white]) {
                    Moves_manage(moves, move);
                }
            }
            
            Index side_movesto[2] = { move.from + 7 * dir, move.from + 9 * dir };
            for (int i = 0; i < 2; ++i) {
                move.to = side_movesto[i];
                if (move.to >= 0 && move.to < 64
                    && game->fen.board[move.to] != '0'
                    && row_from_index(move.to) != row_from_index(move.from)
                    && opponent == isupper(game->fen.board[move.to])
                    ) {
                    Moves_manage(moves, move);
                }
            }
            break; }
        case 'N': {
            Index index_adds[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
            for (int i = 0; i < 8; ++i) {
                move.to = move.from + index_adds[i];
                if (move.to < 0 || move.to > 63) continue;
                if (i < 4 && row_from_index(move.to) > row_from_index(move.from)
                    || i >= 4 && row_from_index(move.to) < row_from_index(move.from)) continue;
                if (i % 2 && col_from_index(move.to) < col_from_index(move.from)
                    || !(i % 2) && col_from_index(move.to) > col_from_index(move.from)) continue;
                if (white && isupper(game->fen.board[move.to])
                   || opponent && islower(game->fen.board[move.to])) continue;
                Moves_manage(moves, move);
            }
            break; }
        }
    }
}

void Moves_manage(Moves* moves, Move move)
{
    if (moves->elements + 1 > moves->capacity) {
        int new_cap = moves->capacity * 2;
        Move* new_dirty = realloc(moves->dirty, sizeof *moves->dirty * new_cap);
        if (new_dirty) moves->dirty = new_dirty;
        int* new_valid = realloc(moves->valid, sizeof *moves->valid * new_cap);
        if (new_valid) moves->valid = new_valid;
        moves->capacity = new_cap;
    }
    moves->dirty[moves->elements] = move;
    moves->elements++;
}

#ifdef __cplusplus

#endif
