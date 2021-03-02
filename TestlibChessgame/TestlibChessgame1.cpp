#include "pch.h"
#include "CppUnitTest.h"
#include "../libchessgame/chessgame.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestlibChessgame
{
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
}
