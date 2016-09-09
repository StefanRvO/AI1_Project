#pragma once
#include "Sokoban_Board.hpp"
#include <vector>
class DeadLockDetector
{
    private:
    public:
        static std::vector<Sokoban_Box *> get_static_deadlock_boxes(Sokoban_Board &board);
        static void bipartite_search_rec(Sokoban_Box *box, std::vector<Sokoban_Box> &visited);


};
