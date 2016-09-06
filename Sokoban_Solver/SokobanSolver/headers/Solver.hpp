#pragma once
#include "Sokoban_Board.hpp"

//Class for solving Sokoban Puzzles


class Solver
{
    public:
        Solver(Sokoban_Board *_board);
        bool solve();
    private:
        Sokoban_Board *board = nullptr;
};
