// import/include
//#define USE_MODULES_CORE
#if defined(USE_MODULES_CORE)
import std.core;
#else
#include <array>
#include <iostream>
#include <set>
#include <map>
#include <forward_list>
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
using Moves = std::forward_list< Move >;

// function definitions
void print_board(Board squares);
Moves moves(const Board& squares, Piece::Color color);

// MAIN
int main()
{
    Board squares;
    std::array< Piece::Name, BOARD_DIMANSION > nopawn_layers{ PN_R, PN_N, PN_B, PN_Q, PN_K, PN_B, PN_N, PN_R };
    std::array< Piece::Name, BOARD_DIMANSION > pawn_layers{ PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P };
    std::array< Piece::Name, BOARD_DIMANSION > null_layers{ PNN, PNN, PNN, PNN, PNN, PNN, PNN, PNN };
    std::array< Piece::Color, BOARD_DIMANSION > color_layer { PC_W, PC_W, PCN, PCN, PCN, PCN, PC_B, PC_B };

    std::array< std::array< Piece::Name, BOARD_DIMANSION >, BOARD_DIMANSION > name_layers{ nopawn_layers, pawn_layers, null_layers, null_layers, null_layers, null_layers, pawn_layers, nopawn_layers };
    for (int color_layer_i = 0, square_index = 0; color_layer_i < BOARD_DIMANSION; color_layer_i++ ) {
        std::array< Piece::Name, BOARD_DIMANSION > name_layer = name_layers[color_layer_i];
        for (int name_layer_i = 0; name_layer_i < BOARD_DIMANSION; name_layer_i++) {
            squares[square_index] = Piece{color_layer[color_layer_i], name_layer[name_layer_i]};
            square_index++;
        }
    }
    squares[29] = Piece{PC_W, PN_Q};
    Piece::Color player = Piece::Color::White;
    print_board(squares);
    Moves available_moves = moves(squares, player);
    int moves_size{0};
    for (auto&move: available_moves) moves_size++;
    std::cout << moves_size<< std::endl;
    for (auto& move: available_moves) {
        std::cout << "from:" << move.from << ", to:" << move.to << std::endl;
    }
}

void print_board(Board squares) {
    
    std::map< Piece::Color, char > color_char{ {PCN, ' '}, {PC_W, '+'}, {PC_B, '-'} };
    std::map< Piece::Name, char > name_char{
            {PNN, ' '}, {PN_B, 'B'}, {PN_K, 'K'},
            {PN_N, 'N'}, {PN_P, 'p'}, {PN_Q, 'Q'}, {PN_R, 'R'} };

    std::array< int, BOARD_DIMANSION > lines{56, 48, 40, 32, 24, 16, 8, 0};
    for (auto line_add : lines) {
        for (int i = line_add; i < line_add + BOARD_DIMANSION; ++i) {
            std::cout << color_char[squares[i].color] << name_char[squares[i].name];
        }
        std::cout << std::endl;
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
        auto push_move = [&move, &moves](){ moves.push_front(move); };
        if (piece.color == player_color) {
            switch (piece.name) {
            case Piece::Name::Pawn: {
                if (index < 56 ) {
                    move.to = index + BOARD_DIMANSION;
                    if (squares[move.to].name == PNN) {
                        push_move();
                        int doublemove_row = player_color == Piece::Color::White ? 2 : 7;
                        move.to = index + BOARD_DIMANSION * 2;
                        if (move.row_from() == doublemove_row && squares[move.to].name == PNN) push_move();
                    }
                    std::array< Move, 2 > side_moves{
                        Move{move.from, index + BOARD_DIMANSION - 1},
                        Move{move.from, index + BOARD_DIMANSION + 1}
                    };
                    for (auto& side_move : side_moves) {
                        move.to = side_move.to;
                        if (move.row_to() > move.row_from() && squares[move.to].color == oppenent_color) push_move();
                    }
                }
                break;
            } // pawn
            case Piece::Name::Rock: {
            queen_rock:
            king_rock:
                std::array< int, 4 > index_adds{8, -8, 1, -1};
                for (int i = 0; i < index_adds.size(); ++i) {
                king_rock_continue:
                    auto f_inboard = [i, &move](){ return (i % 2) ? (move.to >= 0) : (move.to < 64); };
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
                if (piece.name == Piece::Name::Queen) goto rock_queen;
                break;
            } // Rock
            case Piece::Name::Knight: {
                std::array< int, 8 > index_adds{-17, -15, -10, -6, 6, 10, 15, 17};
                for (int i = 0; i < index_adds.size(); ++i) {
                    auto f_inboard = [i, &move]() { return (i < 4) ? (move.to >= 0) : (move.to < 64); };
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
                    auto f_inboard = [i, &move]() { return (i < 2) ? (move.to >= 0) : (move.to < 64); };
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
