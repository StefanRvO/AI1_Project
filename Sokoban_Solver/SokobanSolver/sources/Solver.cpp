#include "Solver.hpp"
#include <iostream>
Solver::Solver(Sokoban_Board *_board)
{
    this->board = _board;
}

bool Solver::solve()
{
    auto moves = this->board->find_possible_moves();
    for(uint32_t i = 0; i < 100000; i++)
    {
        this->board->perform_move(moves[0]);
        moves = this->board->find_possible_moves();
        auto h = this->board->get_heuristic();
        if(h < 0)
        {
            std::cout << this->board->get_board_str() << std::endl;
            return false;
        }
        std::cout << this->board->get_heuristic() << std::endl;
    }
    return false;
}
