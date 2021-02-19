// import/include
//#define USE_MODULES_CORE
#ifdef USE_MODULES_CORE
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
    static constexpr std::array< const char *, 7 > Titles { "Null", "Pawn", "Rock", "Knight", "Bishop", "Queen", "King"};
    Color color;
    Name name;
    void clear() { color = Color::Null; name = Name::Null; }
    bool empty() const { return name == Piece::Name::Null; }
    friend std::ostream& operator<<(std::ostream& os, Piece piece)
    {
        os << Titles[static_cast<int>(piece.name)];
        return os;
    }
};



struct Move {
    static constexpr std::array< char, 8 > letter{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    static constexpr std::array< char, 8 > number{ '8', '7', '6', '5', '4', '3', '2', '1'};
    int from;
    int to;
    static int8_t row(int8_t index) { return (index / BOARD_DIMENSION) + 1; }
    static int8_t col(int8_t index) { return (index % BOARD_DIMENSION) + 1; }
    int8_t row_from() { return Move::row(from); }
    int8_t col_from() { return Move::col(from); }
    int8_t row_to() { return Move::row(to); }
    int8_t col_to() { return Move::col(to); }

    friend std::ostream& operator<<(std::ostream& osm, Move move)
    {
        osm << letter[move.col_from() - 1LL] << number[move.row_from() - 1LL] << "::"
            << letter[move.col_to() - 1LL] << number[move.row_to() - 1LL];
        return osm;
    }
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
using Pieces = std::vector< Piece >;
using Player = Piece::Color;
using Position = int;
using ColorPosition = std::map< Piece::Color, Position >;
using ColorBool = std::map< Piece::Color, bool >;

class Game;
Moves f_moves(const Game& game, Player color);

class Game
{
    static constexpr std::array< Piece::Name, 4 > promotion_variants{ PN_Q, PN_R, PN_B, PN_N };
    Player player_;
    ColorPosition king_position_;
    Position passant_;
    ColorBool kside_castling_;
    ColorBool qside_castling_;
    int fiftymove_rule_;
    Moves notation_;
    Board board_;
    Pieces promotions_;
public:
    bool is_castling_happened;
    Game() :
        is_castling_happened(false),
        player_(PC_W),
        board_(board_init()),
        king_position_({{PC_W, 60}, {PC_B, 4}}),
        passant_(BOARD_SIZE),
        kside_castling_({{PC_W, true}, {PC_B, true}}),
        qside_castling_({{PC_W, true}, {PC_B, true}}),
        fiftymove_rule_(0)
    { }
    Board board() const { return board_; }
    Pieces promotions() const { return promotions_; }

    Moves notation() const { return notation_; }
    void change_player() { player_ = (player_ == PC_W) ? PC_B : PC_W; }
    int total_moves() const { return static_cast<int>(notation_.size() / 2); }
    Player opponent() const { return (player_ == PC_W) ? PC_B : PC_W; }
    Move last_move() const { return notation_.back(); }
    Position passant() const { return passant_; }
    bool is_passant() const { return passant_ != BOARD_SIZE; }
    bool is_fiftymove_rule() const { return fiftymove_rule_ >= 100; }
    bool is_qside_castling(Player player) const { return qside_castling_.at(player); }
    bool is_kside_castling(Player player) const { return qside_castling_.at(player); }

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

    void board_move(Board& board, Move move) const
    {
        Piece &player_piece = board[move.from];
        Piece &opponent_piece = board[move.to];

        if (opponent_piece.color != PCN) opponent_piece.clear();
        else if (player_piece.name == PN_P && move.to == passant_) board[passant_ + 8LL * dir()].clear();
        else if (player_piece.name == PN_K) {
            if (int diff = move.to - move.from; std::abs(diff) == 2) {
                if (diff > 0) std::swap(board[move.to - 1LL], board[move.to + 1LL]);
                else std::swap(board[move.to + 1LL], board[move.to - 2LL]);
            }
        }
        std::swap(player_piece, opponent_piece);
    }

    Moves valid_moves() const {
        Moves valid_moves;
        Moves dirty_moves = f_moves(*this, player_);
        
        for (Move dirty_move : dirty_moves) {
            Position king_position = king_position_.at(player_);
            std::set< Position > invalidated_positions;
            bool valid = true;
            // check is this move a king to determine correct king position
            if (dirty_move.from == king_position) {
                king_position = dirty_move.to;
                // castling
                if (int diff = dirty_move.to - dirty_move.from;  std::abs(diff) == 2) {
                    invalidated_positions.insert(dirty_move.from);
                    invalidated_positions.insert(dirty_move.from + diff - 1);
                    // q side castling additional possible invalidated position
                    if (diff < 0) invalidated_positions.insert(dirty_move.from + diff + 1);
                }
            }
            invalidated_positions.insert(king_position);
            Game dirty_game = *this;
            board_move(dirty_game.board_, dirty_move);
            Moves check_moves = f_moves(dirty_game, opponent());
            for (Move check_move : check_moves) {
                if (invalidated_positions.count(check_move.to)) {
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
        fiftymove_rule_++;
        std::uniform_int_distribution<std::size_t> move_destribution(0, moves.size() - 1);
        Move move = moves[move_destribution(random_generator)];
        if (board_[move.to].name != PNN) {
            fiftymove_rule_ = 0;
        }
        else if (board_[move.to].name == PN_R) {
            Player opp = opponent();
            if (move.col_to() == 1) qside_castling_[opp] = false;
            else if (move.col_to() == 8) kside_castling_[opp] = false;
        }
        else if (board_[move.from].name == PN_K && std::abs(move.from - move.to) == 2) is_castling_happened = true;
        board_move(board_, move);
        notation_.push_back(move);
        // 
        Piece &player_piece = board_[move.to];
        passant_ = BOARD_SIZE;
        if (player_piece.name == PN_P) {
            fiftymove_rule_ = 0;
            // promotion checker
            if (move.row_to() == 1 || move.row_to() == 8) {
                std::uniform_int_distribution<std::size_t> promotion_destribution(0, promotion_variants.size() - 1);
                player_piece = { player_piece.color, promotion_variants[promotion_destribution(random_generator)] };
                promotions_.push_back(player_piece);
            // en passant checker
            } else if (move.row_from() == 7 || move.row_from() == 2) {
                if (move.row_to() == 5 || move.row_to() == 4) {
                    passant_ = move.to + 8 * dir();
                }
            }
        }
        else if (player_piece.name == PN_K) {
            // king moved update position
            king_position_[player_piece.color] = move.to;
            kside_castling_[player_piece.color] = false;
            qside_castling_[player_piece.color] = false;
        }
        if (is_fiftymove_rule()) return false;
        return true;
    }

    int dir(Player player) const { return player == PC_W ? 1 : -1; }  
    int dir() const { return dir(player_); }
    friend std::ostream& operator<<(std::ostream& os, const Game& game)
    {
        if (game.total_moves() > 0) {
            Move last = game.last_move();
            std::array< char, 8 > letter{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
            std::array< char, 8 > number{ '8', '7', '6', '5', '4', '3', '2', '1'};
            os << "=====" << last << "====="  << std::endl;
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
    int max_games = 64;
    Game min_game;
    int min_game_moves = 1000;
    for (int games = 1; games <= max_games; ++games) {
        Game game{};
        while (true) {
            if (!game.move()) break;
            game.change_player();
        }
        if (game.is_castling_happened && min_game_moves > game.total_moves()) {
            min_game_moves = game.total_moves();
            min_game = game;
        }
        if (games % 64 == 0) std::cout << games << std::endl;
        //if (!game.is_fiftymove_rule())
        if (game.is_castling_happened)
            std::cout << "===Moves: " << game.total_moves() << "===" << std::endl << game;
    }

    std::cout << "Min game with castling happened record = " << min_game.total_moves() << std::endl;
    Moves notation = min_game.notation();
    auto halfmoves = notation.size();
    for (Piece promotion : min_game.promotions()) {
        std::cout << "->" << promotion;
    }
    std::cout << std::endl;
    for (int i = 0; i < halfmoves; ++i) {
        if (i % 2 == 0) std::cout << (i / 2 + 1) << ". ";
        std::cout  << notation[i] << "\t";
        if (i % 2) std::cout << std::endl;
    }
}

Moves f_moves(const Game& game, Player player_color)
{
    const Board squares = game.board();
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
                            if (move.to < 64 && (move.row_to() - move.row_from() == 1))
                                if (squares[move.to].color == opponent_color || move.to == game.passant()) push_move();
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
                            if (move.to >= 0 && (move.row_from() - move.row_to() == 1))
                                if (squares[move.to].color == opponent_color && move.to == game.passant()) push_move();
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
                if (game.is_kside_castling(player_color)) {
                    if (squares[move.from + 1LL].empty() && squares[move.from + 2LL].empty()) {
                        move.to = move.from + 2;
                        push_move();
                    }
                }
                if (game.is_qside_castling(player_color)) {
                    if (squares[move.from - 1LL].empty() && squares[move.from - 2LL].empty() && squares[move.from - 3LL].empty()) {
                        move.to = move.from - 2;
                        push_move();
                    }
                }
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
