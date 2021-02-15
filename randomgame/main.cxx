// import/include
//#define USE_MODULES_CORE
#if defined(USE_MODULES_CORE)
import std.core;
#else
#include <array>
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <random>
#include <thread>
#endif

const auto BOARD_DIMANSION = 8;
const auto BOARD_SIZE = 64;

// structs
struct Piece
{
    enum class Color : unsigned { Null = 0, White, Black };
    enum class Name : unsigned { Null = 0, Pawn, Rock, Knight, Bishop, Queen, King };
    Color color;
    Name name;
};
struct Move {
    int from;
    int to;
    static int8_t row(int8_t index) { return (index / BOARD_DIMANSION) + 1; }
    static int8_t col(int8_t index) { return (index % BOARD_DIMANSION) + 1; }
    int8_t row_from() { return Move::row(from); }
    int8_t col_from() { return Move::col(from); }
    int8_t row_to() { return Move::row(to); }
    int8_t col_to() { return Move::col(to); }
};
// constansts
const Piece::Color PCN = Piece::Color::Null; 
const Piece::Color PC_W = Piece::Color::White;
const Piece::Color PC_B = Piece::Color::Black;
const Piece::Name PNN = Piece::Name::Null;
const Piece::Name PN_P = Piece::Name::Pawn;
const Piece::Name PN_R = Piece::Name::Rock;
const Piece::Name PN_N = Piece::Name::Knight;
const Piece::Name PN_B = Piece::Name::Bishop;
const Piece::Name PN_Q = Piece::Name::Queen;
const Piece::Name PN_K = Piece::Name::King;

// aliases
using Board = std::array< Piece, BOARD_SIZE >;
using Moves = std::vector< Move >;

// function definitions
void print_board(const Board& squares);
Moves moves(const Board& squares, Piece::Color color);
void change_player(Piece::Color& current_player);
void board_make_move(Board& squares, Move move);
Board board_init();

// MAIN
int main()
{
    Piece::Color player = Piece::Color::White;
    std::random_device random_device;
    std::mt19937_64 random_move_gen(random_device());
    for (int games = 1; games <= 100; ++games) {
        Board squares = board_init();
        int game_moves_count = 0;
        while (true) {
            print_board(squares);
            Moves player_moves = moves(squares, player);
            if (!player_moves.size()) break;
            std::uniform_int_distribution<> random_move_destribution(0, player_moves.size() - 1);
            Move chosen_move = player_moves[random_move_destribution(random_move_gen)];
            board_make_move(squares, chosen_move);
            game_moves_count++;
            if (game_moves_count > 200) break;
            change_player(player);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        if ((games % 1000) == 0) std::cout << games << std::endl;
    }
}

void change_player(Piece::Color& current_player)
{
    current_player = (current_player == Piece::Color::White) ? Piece::Color::Black : Piece::Color::White;
}

void board_make_move(Board& squares, Move move)
{
    Piece &player_piece = squares[move.from];
    Piece &opponent_piece = squares[move.to];
    if (opponent_piece.name != Piece::Name::Null && player_piece.color != opponent_piece.color) {
        opponent_piece.color = Piece::Color::Null;
        opponent_piece.name = Piece::Name::Null;
    }
    std::swap(player_piece, opponent_piece);
}

Board board_init()
{
    Board squares;
    std::array< Piece::Name, BOARD_DIMANSION > nopawn_layers{ PN_R, PN_N, PN_B, PN_Q, PN_K, PN_B, PN_N, PN_R };
    std::array< Piece::Name, BOARD_DIMANSION > pawn_layers{ PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P };
    std::array< Piece::Name, BOARD_DIMANSION > null_layers{ PNN, PNN, PNN, PNN, PNN, PNN, PNN, PNN };
    std::array< Piece::Color, BOARD_DIMANSION > color_layer { PC_B, PC_B, PCN, PCN, PCN, PCN, PC_W, PC_W };

    std::array< std::array< Piece::Name, BOARD_DIMANSION >, BOARD_DIMANSION > name_layers{ nopawn_layers, pawn_layers, null_layers, null_layers, null_layers, null_layers, pawn_layers, nopawn_layers };
    for (int color_layer_i = 0, square_index = 0; color_layer_i < BOARD_DIMANSION; color_layer_i++ ) {
        std::array< Piece::Name, BOARD_DIMANSION > name_layer = name_layers[color_layer_i];
        for (int name_layer_i = 0; name_layer_i < BOARD_DIMANSION; name_layer_i++) {
            squares[square_index] = Piece{color_layer[color_layer_i], name_layer[name_layer_i]};
            square_index++;
        }
    }
    return squares;
}

//#define UNICODE_CHESS
void print_board(const Board& squares)
{
#ifndef UNICODE_CHESS
    std::cout << "================" << std::endl;
    std::map< Piece::Color, char > color_char{ {PCN, ' '}, {PC_W, '+'}, {PC_B, '-'} };
    std::map< Piece::Name, char > name_char{
        {PNN, ' '}, {PN_B, 'B'}, {PN_K, 'K'}, {PN_N, 'N'},
        {PN_P, 'p'}, {PN_Q, 'Q'}, {PN_R, 'R'}
    };
    auto cout = [&color_char, &name_char, &squares](int i) {
        std::cout << color_char[squares[i].color] << name_char[squares[i].name];
    };
#else
    std::map< Piece::Color, std::map< Piece::Name, std::string > > color_name_char{
        {PCN, {{PNN, "  "}}},
        {PC_W,  {
                    {PN_K, " \u2654"}, {PN_Q, " \u2655"}, {PN_R, " \u2656"},
                    {PN_B, "\u2657"}, {PN_N, "\u2658"}, {PN_P, " \u2659"}
                }
        },
        {PC_B,  {
                    {PN_K, " \u265A"}, {PN_Q, " \u265B"}, {PN_R, " \u265C"},
                    {PN_B, "\u265D"}, {PN_N, "\u265E"}, {PN_P, " \u265F"}
                }
        }
    };
    auto cout = [&color_name_char, &squares](int i) {
        std::cout << color_name_char[squares[i].color][squares[i].name];
    };
#endif
    for (int i = 0; i < BOARD_SIZE; ++i) {
        cout(i);
        if ((i + 1) % 8 == 0) std::cout << std::endl;
    }
}

Moves moves(const Board& squares, Piece::Color player_color)
{
    Moves moves;
    Piece::Color oppenent_color = player_color == Piece::Color::White ? Piece::Color::Black : Piece::Color::White; 
    for(int index = 0, index_current = 0; index < squares.size(); index++) {
        auto piece = squares[index];
        Move move;
        move.from = index;
        auto push_move = [&move, &moves](){ moves.push_back(move); };
        if (piece.color == player_color) {
            switch (piece.name) {
            case Piece::Name::Pawn: {
                if (player_color == Piece::Color::Black) {
                    if (index < 56 ) {
                        move.to = index + BOARD_DIMANSION;
                        if (squares[move.to].name == PNN) {
                            push_move();
                            int doublemove_row = 2;
                            move.to = index + BOARD_DIMANSION * 2;
                            if (move.row_from() == doublemove_row && squares[move.to].name == PNN) push_move();
                        }
                        std::array< Move, 2 > side_moves{
                            Move{move.from, index + BOARD_DIMANSION - 1},
                            Move{move.from, index + BOARD_DIMANSION + 1}
                        };
                        for (auto& side_move : side_moves) {
                            move.to = side_move.to;
                            if (move.to < 64 && (move.row_to() - move.row_from() == 1) && squares[move.to].color == oppenent_color) push_move();
                        }
                    }
                } // white pawn
                else if (player_color == Piece::Color::White) {
                    if (index > 7) {
                        move.to = index - BOARD_DIMANSION;
                        if (squares[move.to].name == PNN) {
                            push_move();
                            int doublemove_row = 7;
                            move.to = index - BOARD_DIMANSION * 2;
                            if (move.row_from() == doublemove_row && squares[move.to].name == PNN) push_move();
                        }
                        std::array< Move, 2 > side_moves{
                            Move{move.from, index - BOARD_DIMANSION - 1},
                            Move{move.from, index - BOARD_DIMANSION + 1}
                        };
                        for (auto& side_move : side_moves) {
                            move.to = side_move.to;
                            if (move.to > 0 && (move.row_from() - move.row_to() == 1) && squares[move.to].color == oppenent_color) push_move();
                        }
                    }
                } // black pawn
                break;
            } // pawn
            case Piece::Name::Rock: {
            queen_rock:
            king_rock:
                std::array< int, 4 > index_adds{8, -8, 1, -1};
                for (int i = 0; i < index_adds.size(); ++i) {
                king_rock_continue:
                    auto f_inboard = [i, &move](){ return (i % 2) ? (move.to >= 0) : (move.to < BOARD_SIZE); };
                    auto f_correct = [i, &move]() { return (i < 2)
                        ? move.col_to() == move.col_from() 
                        : move.row_to() == move.row_from();
                    };
                    move.to = index + index_adds[i];
                    while (f_inboard() && f_correct()) {
                        Piece::Color tocolor = squares[move.to].color;
                        if (tocolor == player_color) break;
                        else if (tocolor == oppenent_color) {
                            push_move();
                            break;
                        }
                        push_move();
                        if (piece.name == Piece::Name::King) {
                            i++;
                            if (i == index_adds.size()) goto rock_king;
                            goto king_rock_continue;
                        }
                        move.to += index_adds[i];
                    }
                }
                if (piece.name == Piece::Name::King) goto rock_king;
                if (piece.name == Piece::Name::Queen) goto rock_queen;
                break;
            } // Rock
            case Piece::Name::Knight: {
                std::array< int, 8 > index_adds{-17, -15, -10, -6, 6, 10, 15, 17};
                for (int i = 0; i < index_adds.size(); ++i) {
                    auto f_inboard = [i, &move]() { return (i < 4) ? (move.to >= 0) : (move.to < BOARD_SIZE); };
                    auto f_updown = [i, &move]() { return (i < 4)
                        ? (move.row_to() < move.row_from())
                        : (move.row_to() > move.row_from());
                    };
                    auto f_leftright = [i, &move]() { return (i % 2)
                        ? (move.col_to() > move.col_from())
                        : (move.col_to() < move.col_from());
                    };
                    move.to = index + index_adds[i];
                    if (f_inboard() && f_updown() && f_leftright()) {
                        Piece::Color tocolor = squares[move.to].color;
                        if (tocolor != player_color) push_move();
                    }
                }
                break;
            } // kNight
            case Piece::Name::Bishop: {
            queen_bishop:
            king_bishop:
                std::array< int, 4 > index_adds{-9, -7, 7, 9};
                for (int i = 0; i < index_adds.size(); ++i) {
                king_bishop_continue:
                    auto f_inboard = [i, &move]() { return (i < 2) ? (move.to >= 0) : (move.to < BOARD_SIZE); };
                    auto f_updown = [i, &move]() { return (i < 2)
                        ? (move.row_to() < move.row_from())
                        : (move.row_to() > move.row_from());
                    };
                    auto f_leftright = [i, &move]() { return (i % 2)
                        ? (move.col_to() > move.col_from())
                        : (move.col_to() < move.col_from());
                    };
                    move.to = index + index_adds[i];
                    while (f_inboard() && f_updown() && f_leftright()) {
                        Piece::Color tocolor = squares[move.to].color;
                        if (tocolor == player_color) break;
                        else if (tocolor == oppenent_color) {
                            push_move();
                            break;
                        }
                        push_move();
                        if (piece.name == Piece::Name::King) {
                            i++;
                            if (i == index_adds.size()) goto bishop_king;
                            goto king_bishop_continue;
                        }
                        move.to += index_adds[i];
                    }
                }
                if (piece.name == Piece::Name::King) goto bishop_king;
                if (piece.name == Piece::Name::Queen) goto bishop_queen;
                break;
            } // Bishop
            case Piece::Name::Queen: {
                goto queen_bishop;
            bishop_queen:
                goto queen_rock;
            rock_queen:
                break;
            } // Queen
            case Piece::Name::King: {
                goto king_bishop;
            bishop_king:
                goto king_rock;
            rock_king:
                break;
            } // King
            }// switch piece name
        } // piece color checker
    } // index cycle
    return moves;
}
