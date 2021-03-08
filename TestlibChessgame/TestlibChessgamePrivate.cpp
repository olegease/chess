#include "pch.h"
#include "TestlibChessgame.hpp"
#include "../libchessgame/chessgame.private.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestlibChessgame::Private
{
    TEST_CLASS(Moves_manage_)
    {
    public:
        TEST_METHOD(max_16_full)
        {
            //RAIIregister reg{ease_chessgame_default_fen()};
            //Id id = reg.id();
            RAII::Register regame{};
            Id id = regame.id();
            Game* game = Game_get(id);
            std::array< Move, 16 > test_moves{};
            auto size = test_moves.size();
            for (size_t i = 0; i < size; ++i) {
                test_moves[i] = { 8, 8};
                Moves_manage(&game->moves, test_moves[i]);
            }
            Assert::AreEqual(16, game->moves.capacity, L"capacity");
            Assert::AreEqual(16, game->moves.elements, L"elements");
            for (size_t i = 0; i < size; ++i) {
                Assert::AreEqual(test_moves[i].from, game->moves.dirty[i].from);
                Assert::AreEqual(test_moves[i].to, game->moves.dirty[i].to);
            }
        }
    };

    TEST_CLASS(Game_moves_dirty_)
    {
    public:
        TEST_METHOD(default_position)
        {
            RAII::Register regame{};
            Id id = regame.id();
            Game* game = Game_get(id);
            Game_moves_dirty(game);
            Assert::AreEqual(32, game->moves.capacity, L"Capacity White");
            Assert::AreEqual(20, game->moves.elements, L"Elements White");
            game->fen.player = ease_chessgame_PLAYER_BLACK;
            Game_moves_dirty(game);
            Assert::AreEqual(64, game->moves.capacity, L"Capacity Black");
            Assert::AreEqual(40, game->moves.elements, L"Elements Black");
        }

        TEST_METHOD(pawns_sidemoves)
        {
            const char tboard[] = "8/8/8/8/8/pppppppp/PPPPPPPP/8 w - - 0 1";
            RAII::Register regame{ease_chessgame_parse_fen(tboard)};
            Game* game = Game_get(regame.id());
            int start = 40;
            int mult = 0;
            while (start > 7) {
                game->fen.player = ease_chessgame_PLAYER_WHITE;
                Game_moves_dirty(game);
                mult++;
                Assert::AreEqual(14 * mult, game->moves.elements, (std::wstring{L"White "} + std::to_wstring(mult)).c_str());

                game->fen.player = ease_chessgame_PLAYER_BLACK;
                Game_moves_dirty(game);
                mult++;
                Assert::AreEqual(14 * mult, game->moves.elements, (std::wstring{L"Black "} + std::to_wstring(mult)).c_str());

                for (Index i = start; i < start + 16; ++i) {
                    Move move = {i, i - 8};
                    Game_move(game, move);
                }
                start -= 8;
            }


        }
        
    };

    TEST_CLASS(row_from_index_)
    {
    public:
        TEST_METHOD(all)
        {
            for (int r = 1, ix = 0; r <= 8; ++r) {
                for (int i = 1; i <= 8; ++i) {
                    Assert::AreEqual(r, row_from_index(ix));
                    ++ix;
                }
            }
        }
    };

    TEST_CLASS(col_from_index_)
    {
    public:
        TEST_METHOD(all)
        {
            for (int i = 1, ix = 0; i <= 8; ++i) {
                for (int c = 1; c <= 8; ++c) {
                    Assert::AreEqual(c, col_from_index(ix));
                    ++ix;
                }
            }
        }
    };
}
