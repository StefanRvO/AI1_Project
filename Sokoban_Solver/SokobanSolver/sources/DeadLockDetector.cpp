#include "DeadLockDetector.hpp"
#include <iostream>
std::vector<Sokoban_Box *> DeadLockDetector::get_static_deadlock_boxes(Sokoban_Board &board)
{   //Uses bipartite deadlocks recognition algorithm to detect static deadlock.
    //Boxes placed in the detected fields will __always__ cause a deadlock.
    //Make copy of board
    auto board_copy = board;
    //Delete all boxes
    for(auto &_box : board_copy.board_boxes)
    {
        auto &box = _box.first;
        if(box->type == Box) box->change_type(Free);
        else if(box->type == Goal_Box) box->change_type(Goal);
    }

    //For all goals, place a box and try to pull it to all fields, mark the visited ones.
    std::vector<Sokoban_Box> visited;
    for(auto &_goal : board_copy.goals)
    {
        //Make tmp board without searhed fields.
        auto tmp_board = board_copy;
        auto goal = &tmp_board.board[_goal->pos.x_pos][_goal->pos.y_pos];
        visited.push_back(*goal);
        goal->change_type(Goal_Searched);
        for(uint8_t _dir = Move_Direction::up; _dir <= Move_Direction::right; _dir++)
        {
            const Move_Direction &dir = (Move_Direction)_dir;
            if(goal->is_pullable(dir))
                bipartite_search_rec(goal->get_neighbour(dir), visited);
        }
    }

    //Mark all visited fields in the board
    for(auto &vbox : visited)
    {
        board_copy.board[vbox.pos.x_pos][vbox.pos.y_pos].change_type(Free_Searched);
    }
    //Find all goal and free fields. these have not been visited, and is thus deadlock squares.
    std::vector<Sokoban_Box *> deadlock_vec;
    for(uint32_t x = 0; x < board.size_x; x++)
        for(uint32_t y = 0; y < board.size_y; y++)
        {
            if(board_copy.board[x][y].type == Free or board_copy.board[x][y].type == Goal)
                deadlock_vec.push_back(&board.board[x][y]);
        }
    return deadlock_vec;
}

void DeadLockDetector::bipartite_search_rec(Sokoban_Box *box, std::vector<Sokoban_Box> &visited)
{
    visited.push_back(*box);
    box->change_type(Free_Searched);
    for(uint8_t _dir = Move_Direction::up; _dir <= Move_Direction::right; _dir++)
    {
        const Move_Direction &dir = (Move_Direction)_dir;
        if(box->is_pullable(dir) and box->get_neighbour(dir)->type != Free_Searched
            and box->get_neighbour(dir)->type != Goal_Searched)
            bipartite_search_rec(box->get_neighbour(dir), visited);
    }
}
