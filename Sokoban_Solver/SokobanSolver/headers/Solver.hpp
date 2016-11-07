#pragma once
#include "Sokoban_Board.hpp"
#include "TransmutationTable.hpp"
#include <queue>

//Class for solving Sokoban Puzzles
bool state_entry_cmp(state_entry* &first, state_entry* &second);


class Solver
{
    public:
        Solver(Sokoban_Board *_board);
        bool solve();
    private:
        std::priority_queue<state_entry *, std::vector<state_entry *>, state_entry> open_list;
        TransmutationTable ttable;
        std::vector<move> moves_from_init;
        std::vector<move> moves_from_goal;

        Sokoban_Board *board = nullptr;
        state_entry *A_star_solve();
        int32_t IDA_star_solve();
        int32_t IDA_search(uint32_t depth, uint32_t g, int32_t bound, state_entry *parent_node, move &last_move);
        void go_to_state(state_entry *init_entry, state_entry *goal_entry);
        std::vector<move> get_path_to_state(state_entry *state);
        void go_too_root_state();

};
