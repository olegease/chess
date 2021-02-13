#define USE_MODULES_CORE
#if defined(USE_MODULES_CORE)
import std.core;
#else
#include <array>
#include <iostream>
#include <set>
#include <map>
#endif

struct Piece
{
    enum class Color : unsigned { Null = 0, White, Black };
    enum class Name : unsigned { Null = 0, Pawn, Rock, Knight, Bishop, Queen, King };
    Color color;
    Name name;
};

const auto BOARD_DIMANSION = 8;
const auto BOARD_SIZE = 64;
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

using board = std::array< Piece, BOARD_SIZE >;

void print_board(board squares) {
    
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

int main()
{
    
    board squares;
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

    print_board(squares);
}
