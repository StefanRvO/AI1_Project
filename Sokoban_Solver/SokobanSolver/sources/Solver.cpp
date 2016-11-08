#include "Solver.hpp"
#include <iostream>
#include "DeadLockDetector.hpp"
#include <boost/range/adaptor/reversed.hpp>
#include <algorithm>




Solver::Solver(Sokoban_Board *_board)
: ttable(1000003)
{
    this->board = _board;
}

bool Solver::solve()
{
    //std::cout << this->board->get_board_str(true) << std::endl;
    /*std::cout << *this->board->player_box << std::endl;
    std::cout << "player box " << *this->board->player_box << std::endl;*/

    /*move test_move = {Move_Direction::left, &this->board->board[8][4]};

    //this->board->perform_move(test_move, false, false);
    std::cout << *this->board->player_box << std::endl;
    std::cout << *this->board << std::endl;
    */

    this->board->calc_reachable(Move_Direction::none);
    //std::cout << this->board->get_reachable_map() << std::endl;
    __attribute__((unused))state_entry *goal_entry = A_star_solve();
    std::cout << "Solved" << std::endl;
    std::cout << *this->board << std::endl;
    auto moves = this->get_path_to_state(goal_entry);
    std::cout << moves.size() << std::endl;
    this->go_too_root_state();
    /*std::cout << *this->board << std::endl;
    this->board->calc_reachable(Move_Direction::none);*/
    //std::cout << *this->board << std::endl;
    //std::cout << this->board->get_reachable_map() << std::endl;

    for(auto &the_move : moves)
    {
        std::cout << the_move << std::endl;
        /*this->board->perform_move(the_move, false, true);
        std::cout << *this->board << std::endl;
        std::cout << this->board->get_reachable_map() << std::endl;*/
    }

    //std::cout << "Player_moves!" << std::endl;
    this->board->calc_reachable(Move_Direction::none);
    auto player_moves = this->board->get_player_moves(moves);
    //std::cout << moves[0] << std::endl;
    std::cout << player_moves.size() << std::endl;
    //for(auto &the_move : player_moves)
    //{
    //    std::cout << the_move << std::endl;
    //}
    this->go_too_root_state();
    this->board->calc_reachable(Move_Direction::none);

    //std::cout << *this->board << std::endl;
    std::cout << this->board->get_move_string(player_moves) << std::endl;
    //{
    //
    //}*/
    return true;
}

state_entry *Solver::A_star_solve()
{
    //Solve the sokoban puzzle using an A* algorithm
    std::list<float> move_costs;
    //Add initial state to ttable and open list
    move init_move = move(Move_Direction::none, this->board->player_box);
    state_entry *this_entry = nullptr;
    int32_t h = 0;
    ttable.check_table(*this->board, 0, &h, init_move, nullptr, 0, this_entry);
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
        //if(node_to_expand->heuristic > 20) std::cout << node_to_expand->heuristic << std::endl;
        this->board->calc_reachable(node_to_expand->last_move.first);
        auto moves = this->board->find_possible_moves();
        move_costs.clear();
        for (__attribute__((unused)) auto &the_move : moves)
        {
            move_costs.push_back(this->board->get_move_cost(the_move));
        }
        for (auto &the_move : moves)
        {
            float move_cost = move_costs.front();
            move_costs.pop_front();
            this->board->perform_move(the_move, false, false);
            if( ttable.check_table(*this->board,node_to_expand->cost_to_state + move_cost, &h, the_move,
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
    if(ttable.check_table(*this->board, g, &h, last_move, parent_node, depth, this_entry) == false)
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
    if(init_entry == goal_entry)
    {
        return;
    }
    moves_from_init.clear();
    moves_from_goal.clear();
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
    for(auto &the_move : moves_from_init)
    {
      this->board->perform_move(the_move, true, false);
    }
    for(auto &the_move :  boost::adaptors::reverse(moves_from_goal))
    {
      this->board->perform_move(the_move, false, false);
    }
    if(moves_from_goal.size() == 0)
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
            this->board->player_box = this->board->initial_player_box;
        }
        else
        {
            //Replay last move
            /*std::cout << *this->board << std::endl;
            std::cout << goal_entry->last_move << std::endl;
            std::cout << *this->board->player_box << std::endl;*/
            this->board->perform_move(goal_entry->last_move, true, false);
            /*std::cout << "test2" << std::endl;
            std::cout << *this->board << std::endl;
            std::cout << goal_entry->last_move << std::endl;*/
            this->board->perform_move(goal_entry->last_move, false, false);
            /*std::cout << "test3" << std::endl;
            std::cout << *this->board << std::endl;
            std::cout << goal_entry->last_move << std::endl;*/
        }
    }

    // std::cout << ttable.get_entry(*board)->full_key << "\t" << goal_entry->full_key << std::endl;
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

void Solver::go_too_root_state()
{
    state_entry *this_state = this->ttable.get_entry(*this->board);
    state_entry *init_state = this_state;
    while(this_state->parent_entry != nullptr)
    {
        this_state = this_state->parent_entry;
    }
    this->go_to_state(init_state, this_state);
}
