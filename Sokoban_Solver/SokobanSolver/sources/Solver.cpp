#include "Solver.hpp"
#include <iostream>
#include "DeadLockDetector.hpp"
#include <boost/range/adaptor/reversed.hpp>
#include <algorithm>



std::ostream& operator<<(std::ostream& os, const move& the_move)
{
    os << "Move: (Dir:" << the_move.first << ", "<< *the_move.second << ")";
    return os;
}

Solver::Solver(Sokoban_Board *_board)
: ttable(1000003)
{
    this->board = _board;
}

bool Solver::solve()
{

    auto dead_fields = DeadLockDetector::get_static_deadlock_boxes(*this->board);
    //std::cout << dead_fields.size() << std::endl;
    //std::cout << *this->board << std::endl;
    for(auto &field : dead_fields)
    {
        //std::cout << *field << std::endl;
        switch(field->type)
        {
            case Free: field->change_type(DeadLock_Zone_Free);
                break;
            case Player: field->change_type(DeadLock_Zone_Player);
                break;
            default: break;
        }
    }
    this->board->calc_reachable(Move_Direction::none);
    std::cout << *this->board << std::endl;
    for (auto &the_move : this->board->find_possible_moves())
    {
        std::cout << the_move << "\t" << this->board->get_move_cost(the_move) << std::endl;
    }
    std::cout << this->board->get_board_str(true);
    //return false;

    //__attribute__((unused)) int32_t solve_result = this->IDA_star_solve();
    state_entry *goal_entry = A_star_solve();
    std::cout << "Solved\t" << goal_entry << std::endl;
    std::cout << *this->board << std::endl;
    auto moves = this->get_path_to_state(goal_entry);
    for(auto &the_move : moves)
    {
        std::cout << the_move << std::endl;
    }
    std::cout << moves.size() << std::endl;
    //Go to random state
    state_entry *random_state1 = this->ttable.get_random_entry();
    state_entry *random_state2 = this->ttable.get_random_entry();

    //std::cout << random_state;
    state_entry *this_state = this->ttable.get_entry(*this->board, this->board->upper_left_reachable->pos);
    std::cout << this_state << std::endl;
    std::cout << random_state1 << std::endl;
    std::cout << random_state2 << std::endl;
    go_to_state(this_state, random_state2);
    this->board->calc_reachable(Move_Direction::none);
    this_state = this->ttable.get_entry(*this->board, this->board->upper_left_reachable->pos);
    std::cout << this_state << std::endl;
    //if(solve_result < 0) return false;
    //std::cout << "Solved in " << solve_result << " steps." << std::endl;
    return true;
}

state_entry *Solver::A_star_solve()
{
    //Solve the sokoban puzzle using an A* algorithm

    //Add initial state to ttable and open list
    move init_move = move(Move_Direction::none, this->board->player_box);
    state_entry *this_entry = nullptr;
    int32_t h = 0;
    ttable.check_table(*this->board, this->board->upper_left_reachable->pos, 0, &h, init_move, nullptr, 0, this_entry);
    open_list.push(this_entry);
    state_entry *last_entry = this_entry;
    while(open_list.size())
    {
        //std::cout << this->board << std::endl;
        //Get the top entry on the open list
        state_entry *node_to_expand = open_list.top();
        this->go_to_state(last_entry, node_to_expand);
        open_list.pop();
        if(node_to_expand->heuristic == 0)
            return node_to_expand;
        //Go to this state
        //Calculate children
        for (auto &the_move : this->board->find_possible_moves())
        {
            int32_t move_cost = 1;
            this->board->perform_move(the_move, false, true);
            if( ttable.check_table(*this->board, this->board->upper_left_reachable->pos,
                node_to_expand->cost_to_state + move_cost, &h, the_move,
                node_to_expand, node_to_expand->total_moves + 1, this_entry) == false)
            {
                this->board->perform_move(the_move, true, false);
                continue;
            }

            this->open_list.push(this_entry);
            this->board->perform_move(the_move, true, false);
        }
        last_entry = node_to_expand;
    }
    return nullptr;
}


int32_t Solver::IDA_star_solve()
{
    move init_move = move(Move_Direction::up, &this->board->board[0][0]);
    int32_t bound = this->board->get_heuristic();
    while(true)
    {
        int32_t t = this->IDA_search(0, 0, bound, nullptr, init_move);
        if(t == 0) return bound;
        if(t < 0) return t;
        bound = t;
        std::cout << "bound:\t" << bound << std::endl;
    }
}

int32_t Solver::IDA_search(uint32_t depth, uint32_t g, int32_t bound, __attribute__((unused)) state_entry *parent_node, __attribute__((unused)) move &last_move)
{
    //std::cout << this->board->get_board_str() << std::endl;
    int32_t h = 0;
    state_entry *this_entry = nullptr;
    //std::cout << parent_node << std::endl;
    //std::cout << depth << std::endl;
    if(ttable.check_table(*this->board, this->board->upper_left_reachable->pos, g, &h, last_move, parent_node, depth, this_entry) == false)
    {
//        std::cout << h << "\tFalse!" << std::endl;
        return -1;
    }
//        std::cout << h << "\t" << "True!" << std::endl;
//        h = this->board->get_heuristic();

    int32_t f = g + h;
    //std::cout << " H: " << h << ", G: " << g << std::endl;
    if(f > bound) return f;
    if(h == 0) return 0;
    if(h == -1) return -1;
    int32_t min = 0xFFFFFF;
    for (auto &the_move : this->board->find_possible_moves())
    {
        this->board->perform_move(the_move, false, true);
        int32_t t = this->IDA_search(depth + 1,  g + 1, bound, this_entry, the_move);
        if(t == 0)
        {
            std::cout << the_move << std::endl;
            std::cout << *this->board << std::endl;
            this->board->perform_move(the_move, true, false);
            return 0;
        }
        this->board->perform_move(the_move, true, false);
        if(t < 0) continue;
        if(t < min) min = t;
    }
    return min;
}

void Solver::go_to_state(state_entry *init_entry, state_entry *goal_entry)
{   //Travel the tree from the init entry to the goal entry by traversing the tree saved in the transmutation table.
    //Create a vector for saving the needed moves
    if(init_entry == goal_entry) return;
    std::vector<move> moves_from_init;
    std::vector<move> moves_from_goal;
    //Loop while full_key is not the same (we are not in the same state!)
    while(init_entry->full_key != goal_entry->full_key)
    {
      //std::cout << goal_entry->full_key << "\t" << init_entry->full_key << std::endl;
      //std::cout << goal_entry << "\t" << init_entry << std::endl;
      //std::cout << goal_entry->total_moves << "\t" << init_entry->total_moves << std::endl;
      //Find the deepest entry and backstep
      if(goal_entry->total_moves < init_entry->total_moves)
      {
          moves_from_init.push_back(init_entry->last_move);
          assert(init_entry->parent_entry != nullptr);
          init_entry = init_entry->parent_entry;
      }
      else
      {
          moves_from_goal.push_back(goal_entry->last_move);
          assert(goal_entry->parent_entry != nullptr);
          goal_entry = goal_entry->parent_entry;
      }
    }
    for(auto &move : moves_from_init)
    {
      this->board->perform_move(move, true, false);
    }
    for(auto &move :  boost::adaptors::reverse(moves_from_goal))
    {
      this->board->perform_move(move, false, false);
    }
    Move_Direction *last_move_dir = nullptr;
    Move_Direction none_move = Move_Direction::none;
    if(moves_from_goal.size())
    last_move_dir = &moves_from_goal.front().first;
    else
    {
    //We have not performed a forward move, which means that the player might not be at the correct position
    //This will make problems for out move cost calculation
    //We replay the last move.
    //We can however not do this if the goal has no parent node, i.e., it was the initial start position.
    //If that is the case, just magically transport the player to the correct position, which was saved before
    //Manipulating the board.
    if(goal_entry->parent_entry == nullptr)
    {
        //This is the genisis node, automagically move player to the correct box.
        this->board->initial_player_box->change_types_in_move(*this->board->player_box, *this->board->initial_player_box);
        last_move_dir = &none_move;
    }
    else
    {
        //Replay last move
        this->board->perform_move(goal_entry->parent_entry->last_move, true, false);
        this->board->perform_move(goal_entry->parent_entry->last_move, false, false);
        last_move_dir = &goal_entry->parent_entry->last_move.first;
    }


    this->board->calc_reachable(*last_move_dir);
    // std::cout << ttable.get_entry(*board, board->upper_left_reachable->pos )->full_key << "\t" << goal_entry->full_key << std::endl;

    }
}

std::vector<move> Solver::get_path_to_state(state_entry *state)
{
    std::vector<move> moves;
    while(state->total_moves != 0)
    {
        moves.push_back(state->last_move);
        state = state->parent_entry;
    }
    reverse(moves.begin(), moves.end());
    return moves;
}
