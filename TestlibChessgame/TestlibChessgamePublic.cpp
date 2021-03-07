#include "pch.h"
#include "CppUnitTest.h"
#include "../libchessgame/chessgame.h"
#include <string>
#include <map>
#include <array>
#include <cstring>
#include <atlbase.h>
#include <atlconv.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestlibChessgame::Public
{
    using Fen = ease_chessgame_Fen;
    TEST_CLASS(default_fen)
    {
    public:
        TEST_METHOD(all)
        {
            ease_chessgame_Fen fen = ease_chessgame_default_fen();
            Assert::AreEqual(fen.board[0], 'r');
            Assert::AreEqual(fen.board[1], 'n');
            Assert::AreEqual(fen.board[2], 'b');
            Assert::AreEqual(fen.board[3], 'q');
            Assert::AreEqual(fen.board[4], 'k');
            Assert::AreEqual(fen.board[5], 'b');
            Assert::AreEqual(fen.board[6], 'n');
            Assert::AreEqual(fen.board[7], 'r');
            for (int i = 0; i < ease_chessgame_BOARD_DIMENSION; ++i) Assert::AreEqual(fen.board[8+i], 'p');
            for (int i = 0; i < ease_chessgame_BOARD_DIMENSION * 4; ++i) Assert::AreEqual(fen.board[16+i], '0');
            for (int i = 0; i < ease_chessgame_BOARD_DIMENSION; ++i) Assert::AreEqual(fen.board[48+i], 'P');
            Assert::AreEqual(fen.board[56], 'R');
            Assert::AreEqual(fen.board[57], 'N');
            Assert::AreEqual(fen.board[58], 'B');
            Assert::AreEqual(fen.board[59], 'Q');
            Assert::AreEqual(fen.board[60], 'K');
            Assert::AreEqual(fen.board[61], 'B');
            Assert::AreEqual(fen.board[62], 'N');
            Assert::AreEqual(fen.board[63], 'R');
            Assert::AreEqual(fen.player, ease_chessgame_PLAYER_WHITE);
            Assert::AreEqual(fen.enpassant, (int8_t)ease_chessgame_BOARD_SIZE);
            Assert::AreEqual(fen.halfclock, (int8_t)0);
            Assert::AreEqual(fen.fullmoves, (int16_t)1);
            Assert::AreEqual(fen.flags[ease_chessgame_FLAG_WHITE_KSIDE_CASTLING], true);
            Assert::AreEqual(fen.flags[ease_chessgame_FLAG_WHITE_QSIDE_CASTLING], true);
            Assert::AreEqual(fen.flags[ease_chessgame_FLAG_BLACK_KSIDE_CASTLING], true);
            Assert::AreEqual(fen.flags[ease_chessgame_FLAG_BLACK_QSIDE_CASTLING], true);
        }

    };

    TEST_CLASS(parse_fen)
    {
    public:
        TEST_METHOD(default_position) {
            ease_chessgame_Fen defen = ease_chessgame_default_fen();
            ease_chessgame_Fen testfen = ease_chessgame_parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            for (int i = 0; i < ease_chessgame_BOARD_SIZE; ++i) {
                Assert::AreEqual(defen.board[i], testfen.board[i]);
            }
            Assert::AreEqual(defen.player, testfen.player);
            Assert::AreEqual(defen.flags[0], testfen.flags[0]);
            Assert::AreEqual(defen.flags[1], testfen.flags[1]);
            Assert::AreEqual(defen.flags[2], testfen.flags[2]);
            Assert::AreEqual(defen.flags[3], testfen.flags[3]);
            Assert::AreEqual(defen.enpassant, testfen.enpassant);
            Assert::AreEqual(defen.halfclock, testfen.halfclock);
            Assert::AreEqual(defen.fullmoves, testfen.fullmoves);
        }
        TEST_METHOD(default_plus_enpassant_e3) {
            Fen testfen = ease_chessgame_parse_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
            Assert::AreEqual('b', testfen.player);
            Assert::AreEqual((int8_t)44, testfen.enpassant);
            Assert::AreEqual('0', testfen.board[52]);
            Assert::AreEqual('P', testfen.board[36]);
        }

        TEST_METHOD(castling_all)
        {
            std::map< std::string, std::array< bool, 4 > > castlings {
                {"KQkq",    {true,  true,   true,   true}},
                {"KQk",     {true,  true,   true,   false}},
                {"KQq",     {true,  true,   false,  true}},
                {"Kkq",     {true,  false,  true,   true}},
                {"Qkq",     {false, true,   true,   true}},
                {"KQ",      {true,  true,   false,  false}},
                {"Kk",      {true,  false,  true,   false}},
                {"Kq",      {true,  false,  false,  true}},
                {"Qk",      {false, true,   true,   false}},
                {"Qq",      {false, true,   false,  true}},
                {"kq",      {false, false,  true,   true}},
                {"K",       {true,  false,  false,  false}},
                {"Q",       {false, true,   false,  false}},
                {"k",       {false, false,  true,   false}},
                {"q",       {false, false,  false,  true}},
                {"-",       {false, false,  false,  false}}
            };
            for (auto& pair : castlings) {
                char bufbeg[128] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w ";
                char bufmid[16] = "";
                char bufend[] = " - 0 1";
                std::strcat(bufmid, pair.first.c_str());
                std::strcat(bufmid, bufend);
                std::strcat(bufbeg, bufmid);
                ATL::CA2W ca2w(pair.first.c_str());
                std::wstring wstr{ca2w};
                Fen fen = ease_chessgame_parse_fen(bufbeg);
                for (int i = 0; i < 4; ++i) Assert::AreEqual<bool>(pair.second[i], fen.flags[i], wstr.c_str());
            }
        }
    };

    TEST_CLASS(index_from_location)
    {
    public:
        TEST_METHOD(all) {
            const char locs[][3] = {
                "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
                "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
                "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
                "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
                "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
                "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
                "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
                "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
            };
            for (int i = 0; i < ease_chessgame_BOARD_SIZE; ++i) {
                Assert::AreEqual(i, ease_chessgame_index_from_location(locs[i]));
            }
        }
    };

    TEST_CLASS(piece_from_index)
    {
    public:
        TEST_METHOD(default_white_king)
        {
            ease_chessgame_ID id = ease_chessgame_register(ease_chessgame_default_fen());
            if (!id) Assert::Fail(L"Fail to register chessgame");
            for (int i = 0; i < ease_chessgame_BOARD_DIMENSION; ++i) {
                if (i == 60) Assert::AreEqual('K', ease_chessgame_piece_from_index(id, i));
                else Assert::AreNotEqual('K', ease_chessgame_piece_from_index(id, i));
            }

            ease_chessgame_unregister(id);
        }
    };

    TEST_CLASS(is_registered)
    {
    public:
        TEST_METHOD(zero_game_id)
        {
            Assert::AreEqual(false, ease_chessgame_is_registered(0));
        }

        TEST_METHOD(register_unregister_expect_false)
        {
            ease_chessgame_ID id = ease_chessgame_register(ease_chessgame_default_fen());
            if (!id) Assert::Fail(L"Fail to register chessgame");
            ease_chessgame_unregister(id);
            Assert::AreEqual(false, ease_chessgame_is_registered(id));
        }
    };
}
