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
}
