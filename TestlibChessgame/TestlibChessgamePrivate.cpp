#include "pch.h"
#include "CppUnitTest.h"
#include "../libchessgame/chessgame.private.h"
#include <array>
#include <memory>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class RAIIregister
{
    Id id_;
public:
    RAIIregister(Fen fen): id_(ease_chessgame_register(fen)) { }
    int id() const { return id_; }
    ~RAIIregister() { ease_chessgame_unregister(id_); }
};

namespace TestlibChessgame::Private
{
    TEST_CLASS(Moves_manage_)
    {
    public:
        TEST_METHOD(max_16_full)
        {
            //RAIIregister reg{ease_chessgame_default_fen()};
            //Id id = reg.id();
            Id id = ease_chessgame_register(ease_chessgame_default_fen());
            if (!id) Assert::Fail(L"Fail to register chessgame");
            Game* game = get_game(id);
            std::array< Move, 16 > test_moves{};
            auto size = test_moves.size();
            for (size_t i = 0; i < size; ++i) {
                test_moves[i] = { 8, 8};
                Moves_manage(&game->moves, test_moves[i]);
            }
            Assert::AreEqual(16, game->moves.capacity, L"capacity");
            Assert::AreEqual(15, game->moves.elements, L"elements");
            for (size_t i = 0; i < size; ++i) {
                Assert::AreEqual(test_moves[i].from, game->moves.dirty[i].from);
                Assert::AreEqual(test_moves[i].to, game->moves.dirty[i].to);
            }

            ease_chessgame_unregister(id);
        }
    };
}
