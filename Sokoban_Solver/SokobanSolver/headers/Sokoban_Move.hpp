#pragma once
#include "Sokoban_Box.hpp"
#include "enums.hpp"
#include <list>
class Sokoban_Box;

class Sokoban_Move;

class Sokoban_Move
{
    public:
    Move_Direction first = Move_Direction::none;
    Sokoban_Box *second = nullptr;
    Move_Type type = Normal;
    std::list<Sokoban_Move> *macro_move = nullptr;
    public:
        Sokoban_Move(Move_Direction _first, Sokoban_Box *_second, bool ignore_macro = false);

        Sokoban_Move() {};
    private:
};

typedef Sokoban_Move move;
