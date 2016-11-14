#pragma once
#include "Sokoban_Box.hpp"
#include "enums.hpp"
class Sokoban_Box;

class Sokoban_Move
{
    public:
    Move_Direction first = Move_Direction::none;
    Sokoban_Box *second = nullptr;

    Move_Type type = Normal;
    public:
        Sokoban_Move(Move_Direction _first, Sokoban_Box *_second);
        Sokoban_Move() {};
    private:
};

typedef Sokoban_Move move;
