#pragma once
#include "Sokoban_Board.hpp"
#include "TransmutationTable.hpp"

//Class for solving Sokoban Puzzles


class Solver
{
    public:
        Solver(Sokoban_Board *_board);
        bool solve();
    private:
        TransmutationTable ttable;
        Sokoban_Board *board = nullptr;
        int32_t IDA_star_solve();
        int32_t IDA_search(uint32_t g, int32_t bound);

};
