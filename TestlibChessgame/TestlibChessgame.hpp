#pragma once

#include "CppUnitTest.h"
#include "../libchessgame/chessgame.h"

namespace RAII
{
    class Register
    {
        ease_chessgame_ID id_;
        void check_failed_id() const
        {
            using namespace Microsoft::VisualStudio::CppUnitTestFramework;
            if (!id_) Assert::Fail(L"Fail to register chess game.");
        }
    public:
        Register() : id_(ease_chessgame_register(ease_chessgame_default_fen())) { check_failed_id(); }
        Register(ease_chessgame_Fen fen) : id_(ease_chessgame_register(fen)) { check_failed_id(); }
        int id() const { return id_; }
        ~Register() { if (id_) { ease_chessgame_unregister(id_); } }
    };
}
