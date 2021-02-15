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

const auto BOARD_DIMENSION = 8;
const auto BOARD_SIZE = 64;

// structs
struct Piece
{
    enum class Color : unsigned { Null = 0, White, Black };
    enum class Name : unsigned { Null = 0, Pawn, Rock, Knight, Bishop, Queen, King };
    Color color;
    Name name;
    void clear() { color = Color::Null; name = Name::Null; }
};



struct Move {
    int from;
    int to;
    static int8_t row(int8_t index) { return (index / BOARD_DIMENSION) + 1; }
    static int8_t col(int8_t index) { return (index % BOARD_DIMENSION) + 1; }
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
using Player = Piece::Color;
using Position = int;

// function definitions
Moves moves(const Board& squares, Piece::Color color);
void change_player(Piece::Color& current_player);
void board_make_move(Board& squares, Move move);
Board board_init();

class Game
{
    static constexpr std::array< Piece::Name, 4 > promotion_variants{ PN_Q, PN_R, PN_B, PN_N };
    Player player_;
    std::map< Piece::Color, bool > king_moved_{ {PC_W, false}, {PC_B, false} };
    std::map< Piece::Color, bool > lrock_moved_{ {PC_W, false}, {PC_B, false} };
    std::map< Piece::Color, bool > rrock_moved_{ {PC_W, false}, {PC_B, false} };
    Moves notation_;
    Board board_;
    std::vector< Piece > promotions;
public:
    Game() : player_(PC_W), board_(board_init()) { }
    Board board() const { return board_; }
    bool is_king_moved(Player color) { return king_moved_[color]; }
    bool is_king_moved() { return is_king_moved(player_); }

    void king_moved(Player color) { king_moved_[color] = true; }
    void king_moved() { king_moved_[player_] = true; }

    bool is_lrock_moved(Player color) { return lrock_moved_[color]; }
    bool is_lrock_moved() { return lrock_moved_[player_]; }

    void lrock_moved(Player color) { lrock_moved_[color] = true; }
    void lrock_moved() { lrock_moved_[player_] = true; }

    bool is_rrock_moved(Player color) { return rrock_moved_[color]; }
    bool is_rrock_moved() { return rrock_moved_[player_]; }

    void rrock_moved(Player color) { rrock_moved_[color] = true; }
    void rrock_moved() { rrock_moved_[player_] = true; }

    Position king() const {
        Position index = 0;
        for (Piece piece : board_) {
            if (piece.name == PN_K && piece.color == player_) return index;
            index++;
        }
        return BOARD_SIZE;
    }
    void change_player() { player_ = (player_ == PC_W) ? PC_B : PC_W; }
    int total_moves() const { return static_cast<int>(notation_.size() / 2); }
    Player opponent() const { return (player_ == PC_W) ? PC_B : PC_W; }
    Move last_move() const { return notation_.back(); }
    Moves valid_moves() const {
        Moves dirty_moves = ::moves(board_, player_);
        Moves valid_moves;
        Position king_position = king();
        for (Move dirty_move : dirty_moves) {
            bool valid = true;
            Board dirty_board = board();
            Moves check_moves = ::moves(dirty_board, opponent());
            for (Move check_move : check_moves) {
                if (check_move.to == king_position) {
                    valid = false;
                    break;
                }
            }
            if (valid) valid_moves.push_back(dirty_move);
        }
        return valid_moves;
    }

    bool move() {
        std::random_device random_device;
        std::mt19937_64 random_generator(random_device());
        Moves moves = valid_moves();
        if (moves.empty()) return false;
        std::uniform_int_distribution<std::size_t> move_destribution(0, moves.size() - 1);
        Move move = moves[move_destribution(random_generator)];
        Piece &piece_from = board_[move.from];
        Piece &piece_to = board_[move.to];
        // castling checker
        if (piece_from.name == PN_K) king_moved();
        if (piece_from.name == PN_R) {
            if (move.col_from() == 1) lrock_moved();
            if (move.col_from() == 8) rrock_moved();
        }
        // promotion checker
        if (piece_from.name == PN_P) {
            if (move.row_from() == 1 || move.row_from() == 8) {
                std::uniform_int_distribution<std::size_t> promotion_destribution(0, promotion_variants.size() - 1);
                piece_from = { piece_from.color, promotion_variants[promotion_destribution(random_generator)] };
            }
        }
        notation_.push_back(move);
        if (piece_to.color == opponent()) piece_to.clear();
        std::swap(piece_from, piece_to);
        
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Game& game)
    {
        if (game.total_moves() > 0) {
            Move last = game.last_move();
            std::array< char, 8 > letter{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
            std::array< char, 8 > number{ '8', '7', '6', '5', '4', '3', '2', '1'};
            os << "====="
                << letter[last.col_from() - 1LL] << number[last.row_from() - 1LL] << "::"
                << letter[last.col_to() - 1LL] << number[last.row_to() - 1LL] << "====="  << std::endl;
        }
#ifndef UNICODE_CHESS
        std::map< Piece::Color, char > color_char{ {PCN, ' '}, {PC_W, '+'}, {PC_B, '-'} };
        std::map< Piece::Name, char > name_char{
            {PNN, ' '}, {PN_B, 'B'}, {PN_K, 'K'}, {PN_N, 'N'},
            {PN_P, 'p'}, {PN_Q, 'Q'}, {PN_R, 'R'}
        };
        auto cout = [&os, &color_char, &name_char, &game](int i) {
            os << color_char[game.board_[i].color] << name_char[game.board_[i].name];
        };
#else
        std::map< Piece::Color, std::map< Piece::Name, std::string > > color_name_char{
            {PCN, {{PNN, "  "}}},
            {PC_W,  {
                        {PN_K, " \u2654"}, {PN_Q, " \u2655"}, {PN_R, " \u2656"},
                        {PN_B, " \u2657"}, {PN_N, " \u2658"}, {PN_P, " \u2659"}
                    }
            },
            {PC_B,  {
                        {PN_K, " \u265A"}, {PN_Q, " \u265B"}, {PN_R, " \u265C"},
                        {PN_B, " \u265D"}, {PN_N, " \u265E"}, {PN_P, " \u265F"}
                    }
            }
        };
        auto cout = [&os, &color_name_char, &game](int i) {
            os << color_name_char[game.board_[i].color][game.board_[i].name];
        };
#endif
        for (int i = 0; i < BOARD_SIZE; ++i) {
            cout(i);
            if ((i + 1) % 8 == 0) os << std::endl;
        }
        return os;
    }
};

// MAIN
int main()
{
    for (int games = 1; games <= 100; ++games) {
        Game game{};
        std::cout << game;
        while (true) {
            if (!game.move()) break;
            std::cout << game;
            if (game.total_moves() > 100) break;
            game.change_player();
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
    Board squares{};
    std::array< Piece::Name, BOARD_DIMENSION > nopawn_layers{ PN_R, PN_N, PN_B, PN_Q, PN_K, PN_B, PN_N, PN_R };
    std::array< Piece::Name, BOARD_DIMENSION > pawn_layers{ PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P, PN_P };
    std::array< Piece::Name, BOARD_DIMENSION > null_layers{ PNN, PNN, PNN, PNN, PNN, PNN, PNN, PNN };
    std::array< Piece::Color, BOARD_DIMENSION > color_layer { PC_B, PC_B, PCN, PCN, PCN, PCN, PC_W, PC_W };

    std::array< std::array< Piece::Name, BOARD_DIMENSION >, BOARD_DIMENSION > name_layers{ nopawn_layers, pawn_layers, null_layers, null_layers, null_layers, null_layers, pawn_layers, nopawn_layers };
    for (int color_layer_i = 0, square_index = 0; color_layer_i < BOARD_DIMENSION; color_layer_i++ ) {
        std::array< Piece::Name, BOARD_DIMENSION > name_layer = name_layers[color_layer_i];
        for (int name_layer_i = 0; name_layer_i < BOARD_DIMENSION; name_layer_i++) {
            squares[square_index] = Piece{color_layer[color_layer_i], name_layer[name_layer_i]};
            square_index++;
        }
    }
    return squares;
}

Moves moves(const Board& squares, Piece::Color player_color)
{
    Moves moves;
    Piece::Color opponent_color = player_color == Piece::Color::White ? Piece::Color::Black : Piece::Color::White; 
    for(int index = 0, index_current = 0; index < squares.size(); index++) {
        Piece piece = squares[index];
        Move move{index};
        auto push_move = [&move, &moves](){ moves.push_back(move); };
        if (piece.color == player_color) {
            switch (piece.name) {
            case Piece::Name::Pawn: {
                if (player_color == Piece::Color::Black) {
                    if (index < 56 ) {
                        move.to = index + BOARD_DIMENSION;
                        if (squares[move.to].name == PNN) {
                            push_move();
                            int doublemove_row = 2;
                            move.to = index + BOARD_DIMENSION * 2;
                            if (move.row_from() == doublemove_row && squares[move.to].name == PNN) push_move();
                        }
                        std::array< Move, 2 > side_moves{
                            Move{move.from, index + BOARD_DIMENSION - 1},
                            Move{move.from, index + BOARD_DIMENSION + 1}
                        };
                        for (auto& side_move : side_moves) {
                            move.to = side_move.to;
                            if (move.to < 64 && (move.row_to() - move.row_from() == 1) && squares[move.to].color == opponent_color) push_move();
                        }
                    }
                } // white pawn
                else if (player_color == Piece::Color::White) {
                    if (index > 7) {
                        move.to = index - BOARD_DIMENSION;
                        if (squares[move.to].name == PNN) {
                            push_move();
                            int doublemove_row = 7;
                            move.to = index - BOARD_DIMENSION * 2;
                            if (move.row_from() == doublemove_row && squares[move.to].name == PNN) push_move();
                        }
                        std::array< Move, 2 > side_moves{
                            Move{move.from, index - BOARD_DIMENSION - 1},
                            Move{move.from, index - BOARD_DIMENSION + 1}
                        };
                        for (auto& side_move : side_moves) {
                            move.to = side_move.to;
                            if (move.to > 0 && (move.row_from() - move.row_to() == 1) && squares[move.to].color == opponent_color) push_move();
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
                        else if (tocolor == opponent_color) {
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
                        else if (tocolor == opponent_color) {
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
