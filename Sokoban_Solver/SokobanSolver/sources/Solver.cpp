#include "Solver.hpp"
#include <iostream>
Solver::Solver(Sokoban_Board *_board)
{
    this->board = _board;
}

bool Solver::solve()
{
    //std::cout << this->board->goals.size() << std::endl;
    //auto moves = this->board->find_possible_moves();
    //return false;
    /*for(uint32_t i = 0; i < 1000000; i++)
    {
        this->board->perform_move(moves[0]);
        std::cout << this->board->get_board_str() << std::endl;
        this->board->perform_move(moves[0], true);
        std::cout << this->board->get_board_str() << std::endl;

    }*/

    uint32_t solve_result = this->IDA_star_solve();
    if(solve_result == 0xFFFFFFFF) return false;
    std::cout << "Solved in " << solve_result << " steps." << std::endl;
    return true;
}

int32_t Solver::IDA_star_solve()
{
    int32_t bound = this->board->get_heuristic();
    while(true)
    {
        uint32_t t = this->IDA_search(0, bound);
        if(t == 0) return bound;
        if(t == 0xFFFFFF) return t;
        bound = t;
        std::cout << bound << std::endl;
    }
}

int32_t Solver::IDA_search(uint32_t g, int32_t bound)
{
    //std::cout << this->board->get_board_str() << std::endl;
    int32_t h =  this->board->get_heuristic();
    int32_t f = g + h;
    //std::cout << " H: " << h << ", G: " << g << std::endl;
    if(f > bound) return f;
    if(h == 0) return 0;
    int32_t min = 0xFFFFFF;
    for (auto &the_move : this->board->find_possible_moves())
    {
        this->board->perform_move(the_move);
        int32_t t = this->IDA_search( g + 1, bound);
        if(t == 0) return 0;
        this->board->perform_move(the_move, true);
        if(t < 0) continue;
        if(t < min) min = t;
    }
    return min;
}
