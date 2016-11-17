#include <boost/algorithm/string.hpp> //boost::split and boost::is_any_of
#include "Sokoban_Board.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <random>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <boost/range/adaptor/reversed.hpp>
#include <cctype>
#include <cstdio>
#include "DeadLockDetector.hpp"
#include "TunnelMacroCreator.hpp"


uint8_t get_digits(uint32_t x)
{
    uint8_t len = 1;
    while( x/=10 ) len++;
    return len;
}

Box_Type Sokoban_Board::parse_char(char chr)
{ //Parse a single character from the board string.
  //Return a Box_Type
    switch(chr)
    {
        case '#': return Wall;
        case '*': return Goal_Box;
        case '.': return Goal;
        case '$': return Box;
        case '@': return Player;
        case '+': return Player_On_Goal;
        case ' ':
        case '-':
        case '_': return Free;
        default:
        throw std::runtime_error(std::string("Trying to parse malformated string!\nProblematic character: ") + chr);
    }
}

bool Sokoban_Board::is_freeze_deadlocked()
{
    for(auto &box : this->board_boxes)
    {
        this->frozen++;
        if(box.first->is_freeze_deadlocked(frozen))
        {
            return true;
        }
    }
    return false;

}
char Sokoban_Board::get_box_char(const Sokoban_Box &box)
{
    const Box_Type &type = box.type;
    switch(box.tunnel_type)
    {
        case None:
            break;
        case One_Way:
            return '-';
        case Two_Way:
            return '=';
    }

    switch(type)
    {
        case Wall:            return '#';
        case Goal_Box:        return '*';
        case Goal:            return '.';
        case Box:             return '$';
        case DeadLock_Zone_Player: return '%';
        case Player:          return '@';
        case Player_On_Goal:  return '+';
        case DeadLock_Zone_Free:   return '&';
        case Free:            return ' ';
        default:
        throw std::runtime_error(std::string("Trying to convert invalid type to char.\nThis should not happen! Type was" + std::to_string((int)type)));
    }

}

std::vector <Sokoban_Box> Sokoban_Board::parse_row(const std::string &row_str, uint32_t y_pos)
{   //Parse the row given in the string.
    //Return a vector of Sokoban_Box's
    //Returns an empty vector on error.
    //Allocate the row
    std::vector <Sokoban_Box> boxes;
    for(uint32_t i = 0; i < row_str.size(); i++)
    {
        const char &this_char = row_str[i];
        if(this_char >= '0' && this_char <= '9')
        {
            //Return empty vector if this is last char, as that is illegal.
            if(i + 1 == row_str.size()) return std::vector <Sokoban_Box>();

            //This is run length encoding. Fetch the next type and add the given number of those.
            uint32_t num = this_char - '0';
            Box_Type type = Sokoban_Board::parse_char(row_str[++i]);
            for(uint32_t j = 0; j < num; j++)
            {
                Position pos = {(uint32_t)boxes.size(), y_pos};
                boxes.push_back(Sokoban_Box(type, pos));
            }
        }
        else
        {
            Box_Type type = Sokoban_Board::parse_char(this_char);
            Position pos = {(uint32_t)boxes.size(), y_pos};
            boxes.push_back(Sokoban_Box(type, pos));
        }
    }
    return boxes;
}



Sokoban_Board::~Sokoban_Board()
{
    if(this->r) delete[] r;
}


//Copy constructor
Sokoban_Board::Sokoban_Board(Sokoban_Board &_board)
: rand_gen(_board.rand_gen), gen(_board.gen), cost_matrix(_board.cost_matrix)
{
    this->board = _board.board;
    this->size_x = _board.size_x;
    this->size_y = _board.size_y;
    this->upper_left_reachable = &this->board[_board.upper_left_reachable->pos.x_pos][_board.upper_left_reachable->pos.y_pos];
    //Create_neighbour pointers
    this->populate_neighbours();
}

Sokoban_Board::Sokoban_Board(std::string &board_str)
: rand_gen(1, 0x0FFFFFFFFFFFFFFF),  gen(this->rd()), cost_matrix(1,1)

{
    //Split into the rows.
    std::vector<std::string> rows;
    boost::split(rows, board_str, boost::is_any_of("\n,|"));
    //create vector representation of board.
    std::vector < std::vector<Sokoban_Box> > board_vec;
    //Create the rows and add to board_vec, and find the max x size.
    uint32_t max_x = 0;
    for(auto &row : rows)
    {
        auto new_row = Sokoban_Board::parse_row(row, board_vec.size());
        if(new_row.size() > max_x) max_x = new_row.size();
        board_vec.push_back(new_row);
    }
    this->size_x = max_x;
    this->size_y = rows.size();
    //Alloc and fill the board.
    this->board = std::vector< std::vector <Sokoban_Box> >(max_x);
    for(uint32_t x = 0; x < this->size_x; x++)
    {
        auto &collumn = this->board[x];
        for(uint32_t y = 0; y < this->size_y; y++)
        {
            if(board_vec[y].size() < x + 1) //Add a wall if the given string has omitted it at the end.
                collumn.push_back(Sokoban_Box(Wall, {x, y}));
            else
                collumn.push_back(board_vec[y][x]);
        }
    }
    this->populate_neighbours();
    this->calc_reachable(Move_Direction::none);
    auto dead_fields = DeadLockDetector::get_static_deadlock_boxes(*this);
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
//    std::cout << get_board_str(true) << std::endl;
    cost_matrix = dlib::matrix<int>(this->board_boxes.size(), this->board_boxes.size());
    this->make_wavefront_maps();
    TunnelMacroCreator macroCreator(this);
    macroCreator.compute_macros();
    std::cout << *this << std::endl;
}

void Sokoban_Board::make_wavefront_maps()
{
    //Temporarily remove player and boxes
    std::vector<Box_Type> types;
    for(auto &box : this->board_boxes)
    {
        types.push_back(box.first->type);
        box.first->type = Box_Type::Free;
    }
    types.push_back(this->player_box->type);
    switch(this->player_box->type)
    {
        case Player_On_Goal:
        case Player:
            this->player_box->type = Box_Type::Free;
            break;
        case DeadLock_Zone_Player:
            this->player_box->type = Box_Type::DeadLock_Zone_Free;
            break;
        default:
            assert(false);
    }
    for(uint32_t x = 1; x < this->size_x -1; x++ )
    {
        //Make sure only to make a wavefront inside the map
        uint32_t y_limit_low = 0;
        uint32_t y_limit_high = this->size_y;
        while(y_limit_low < this->size_y)
            if(this->board[x][y_limit_low++].is_solid()) break;
        y_limit_low--;
        while(y_limit_high > 0) if(this->board[x][--y_limit_high].is_solid()) break;
        y_limit_high++;

        for(uint32_t y = y_limit_low + 1; y < y_limit_high; y++ )
        {
            if(this->board[x][y].is_solid()) continue;
            this->calculate_cost_map(this->board[x][y]);
        }

    }

    //Restore boxes and player
    this->player_box->type = types.back();
    types.pop_back();
    uint32_t i = 0;
    for(auto &box : this->board_boxes)
    {
        box.first->type = types[i++];
    }
}

std::string Sokoban_Board::get_board_str(bool with_coords) const
{
    std::string board_str = "";
    for(uint32_t y = 0; y < this->size_y; y++)
    {

        std::string row_str;
        if(with_coords)
        {
            row_str = std::to_string(y);
            while(row_str.size() < (uint32_t)get_digits(this->size_y) + 1) row_str += " ";
        }
        for(uint32_t x = 0; x < this->size_x; x++)
        {
            row_str += Sokoban_Board::get_box_char(this->board[x][y]);
        }
        board_str += row_str;
        board_str += "\n";
    }
    if(with_coords)
    {
        board_str += "\n";
        for(uint32_t i = 0; i < (uint32_t)get_digits(this->size_y) + 1; i++) board_str += " ";
        for(uint32_t i = 0; i < this->size_x; i++)
            board_str += std::to_string(i % 10);
        board_str += "\n";
        if(this->size_x > 10)
        {
            for(uint32_t i = 0; i < (uint32_t)get_digits(this->size_y) + 1; i++) board_str += " ";
            for(uint32_t i = 0; i < this->size_x; i++)
                if(i%10)  board_str += " ";
                else      board_str += std::to_string(i/10);
        }
        board_str += "\n";
    }
    return board_str;
}

void Sokoban_Board::populate_neighbours()
{
    for(uint32_t x = 0; x < this->size_x; x++)
    {
        for(uint32_t y = 0; y < this->size_y; y++)
        {
            Sokoban_Box *up = nullptr;
            Sokoban_Box *down = nullptr;
            Sokoban_Box *left = nullptr;
            Sokoban_Box *right = nullptr;
            if(y > 0) up = &this->board[x][y - 1];
            if(y + 1 < this->size_y) down = &this->board[x][y + 1];
            if(x > 0) left = &this->board[x - 1][y];
            if(x + 1 < this->size_x) right = &this->board[x + 1][y];
            this->board[x][y].set_neighbours(up, down, left, right);
            //Set player box if this is the one.
            if(this->board[x][y].type == Player or this->board[x][y].type == Player_On_Goal or this->board[x][y].type == DeadLock_Zone_Player)
                this->player_box = &this->board[x][y];
            else if(this->board[x][y].type == Box or this->board[x][y].type == Goal_Box)
                this->board_boxes.insert(std::pair<Sokoban_Box *,Sokoban_Box *>(&this->board[x][y], &this->board[x][y]));
            if(this->board[x][y].type == Goal or this->board[x][y].type == Goal_Box or this->board[x][y].type == Player_On_Goal)
                this->goals.push_back(&this->board[x][y]);
        }
    }
    this->initial_player_box = this->player_box;
}

bool Sokoban_Board::is_solved()
{
    for(auto &box_tmp : this->board_boxes)
    {
        Sokoban_Box &box = *box_tmp.first;
        if(box.type != Box_Type::Goal_Box)
            return false;
    }
    return true;
}

std::vector<move> Sokoban_Board::find_possible_moves()
{
    std::vector<move> moves;
    moves.reserve(20);
    for(auto &box_tmp : this->board_boxes)
    {
        Sokoban_Box &box = *box_tmp.first;
        for(uint8_t _dir = Move_Direction::up; _dir <= Move_Direction::right; _dir++)
        {
            const Move_Direction &dir = (Move_Direction)_dir;
            Move_Direction reverse_dir = get_reverse_direction(dir);
            if(box.is_moveable(dir) && this->is_reachable(box.get_neighbour(reverse_dir)))
            {
                auto the_move = move(dir, &box);
                if(the_move.type != Invalid)
                    moves.push_back(the_move);
            }
        }
    }
    return moves;
}


void Sokoban_Board::perform_move(const move &the_move, bool reverse, bool recalculate,
    bool ignore_macro)
{
    if(the_move.type == Macro && !ignore_macro)
    {
        //std::cout << "macro " << the_move << " " << reverse << " " << ignore_macro << std::endl;

        if(reverse)
        {
            for(Sokoban_Move &macro_move : boost::adaptors::reverse(*the_move.macro_move))
            {
                this->perform_move(macro_move, true, false);
            }
            this->perform_move(the_move, true, recalculate, true);
            return;
        }
        else
        {
            this->perform_move(the_move, false, false, true);
            for(Sokoban_Move &macro_move : *the_move.macro_move)
            {
                this->perform_move(macro_move, false, false);
            }
            if(recalculate)
            {
                this->calc_reachable(the_move.macro_move->back().first);
            }
            return;
        }
    }
    //std::cout << "non_macro " << the_move << " " << reverse << " " << ignore_macro << std::endl;
    /*static uint64_t lol = 0;
    lol++;
    if(lol % 10000 == 0) std::cout << lol << std::endl;*/
    Sokoban_Box *start_pos  = nullptr;
    Sokoban_Box *end_pos    = nullptr;
    Sokoban_Box *box = the_move.second;
    if(reverse == false)
    {
        start_pos = box;
        Sokoban_Box::move(box, this->player_box, the_move.first, reverse);
        end_pos = box;
    }
    else
    {
        //Perform move
        start_pos = box->get_neighbour(the_move.first);
        end_pos = box;
        /*std::cout << *start_pos << std::endl;
        std::cout << *end_pos << std::endl;*/

        Sokoban_Box::move(box, this->player_box, the_move.first, reverse);
    }
    #ifndef NDEBUG
    auto start_size = this->board_boxes.size();
    #endif
    this->board_boxes.erase(start_pos);
    this->board_boxes.insert(std::pair<Sokoban_Box *,Sokoban_Box *>(end_pos, end_pos));
    //Recalculate reachable zone
    if(recalculate)
        this->calc_reachable(the_move.first);
    assert(start_size == this->board_boxes.size());
}

float Sokoban_Board::get_heuristic()
{
    if(this->is_freeze_deadlocked()) return std::numeric_limits<float>::max();

    float h = this->compute_minimum_cost_matching() * (PUSH_COST + MOVE_COST +
        std::min({LEFT_COST, RIGHT_COST, FORWARD_COST, BACKWARD_COST}));
    return h;
}

float Sokoban_Board::compute_minimum_cost_matching()
{   //Compute the minimum cost matching for each box in relation to each goal.
    //This could be used as heuristic function.
    uint32_t i = 0;
    for(auto &box_pair : this->board_boxes)
    {
        uint32_t j = 0;
        auto &box = box_pair.first;
        for(auto &goal : this->goals)
        {
            cost_matrix(i,j) = -box->get_cost_to_box(*goal);
            j++;
        }
        i++;
    }
    //std::cout << std::endl << cost_matrix << std::endl;
    int minimum_matched_cost = 0;
    std::vector<long> assignment = dlib::max_cost_assignment(cost_matrix);
    i = 0;
    for(__attribute__((unused)) auto &box_pair : this->board_boxes)
    {
        uint32_t j = 0;
        auto &box = box_pair.first;
        for(auto &goal : this->goals)
        {
            if(assignment[i] == j)
            {
                minimum_matched_cost += box->get_cost_to_box(*goal);
                //std::cout << j << std::endl;
            }
            j++;
        }
        i++;
    }

    return minimum_matched_cost;
}
bool Sokoban_Board::is_reachable(Sokoban_Box *box) const
{
    if(box->cost_to_box != std::numeric_limits<float>::max() && !box->is_solid())
        return true;
    return false;
}

void Sokoban_Board::calc_reachable(Move_Direction last_move_dir)
{
    //std::cout << "Calculated reachable" << std::endl;
    //Zero out the map
    this->upper_left_reachable = this->player_box;
    //Clear reachable map
    for(uint32_t x = 0; x < this->size_x; x++)
        for(uint32_t y = 0; y < this->size_y; y++)
        {
            this->board[x][y].cost_to_box = std::numeric_limits<float>::max();
            this->board[x][y].closed = false;
            this->board[x][y].parent_node = nullptr;
        }
    //Empty the priority queue
    reachable_open_list.clear();

    Position &player_pos = this->player_box->pos;
    //We add this to all cost, as it is the cheapest possible move (we only consider moves which are pushing a box.)
    this->board[player_pos.x_pos][player_pos.y_pos].cost_to_box = PUSH_COST;
    this->board[player_pos.x_pos][player_pos.y_pos].move_dir = last_move_dir;

    //Insert everything into queue(but not at the edges, this will never be part of the reacable space)
    for(uint32_t x = 1; x < this->size_x - 1; x++)
        for(uint32_t y = 1; y < this->size_y - 1; y++)
            {
                reachable_open_list.insert(&this->board[x][y]);
            }

    while(reachable_open_list.size())
    {
        //Get top entry.
        auto top_itt = reachable_open_list.begin();
        Sokoban_Box *top = *top_itt;
        if(top->cost_to_box == std::numeric_limits<float>::max())
            break;
        reachable_open_list.erase(top_itt);
        top->closed = true;
        //std::cout << *top << std::endl;
        if(top->is_solid())
            continue;
        if(upper_left_reachable->pos < top->pos) upper_left_reachable = top;
        //std::cout << "Poped\t" << reachable_open_list.size() <<  std::endl;
        calc_reachable_helper(top->nb_up, top, MOVE_COST, Move_Direction::up);
        calc_reachable_helper(top->nb_down, top, MOVE_COST, Move_Direction::down);
        calc_reachable_helper(top->nb_left, top, MOVE_COST, Move_Direction::left);
        calc_reachable_helper(top->nb_right, top, MOVE_COST, Move_Direction::right);
    }
}

void Sokoban_Board::calc_reachable_helper(Sokoban_Box *neighbour, Sokoban_Box *current,
    float edge_cost, Move_Direction move_dir)
{
    if(neighbour->closed || neighbour->is_solid())
    {
        return;
    }
    //calculate turn cost

    //We can only have forward, left and right cost, as the robot will not reverse direction
    //When it have not pushed a box.
    //We may run into problems here as we can modify last moves of current later?
    float turn_cost = this->get_turn_direction_cost(current->move_dir, move_dir);

    float new_distance = current->cost_to_box + edge_cost + turn_cost;
    //std::cout << "new cost" << new_distance << std::endl;
    if(new_distance < neighbour->cost_to_box)
    {
        //Remove and insert into queue
        //std::cout << "replaces" << std::endl;
        auto itt = reachable_open_list.find(neighbour);
        if(itt == reachable_open_list.end())
        {
            assert(false);
            reachable_open_list.insert(neighbour);
            return;
        }
        neighbour->cost_to_box = new_distance;
        neighbour->parent_node = current;
        neighbour->move_dir = move_dir;
        /*
        Position check_pos = {1,2};
        if(current->pos.x_pos == check_pos.x_pos && current->pos.y_pos == check_pos.y_pos)
            std::cout << "Set parent node for " << *neighbour << std::endl;*/
        reachable_open_list.erase(itt);
        //std::cout << reachable_open_list.size() << std::endl;
        reachable_open_list.insert(neighbour);
        //std::cout << reachable_open_list.size() << std::endl;
    }
}
float Sokoban_Board::get_turn_direction_cost(Move_Direction last_dir, Move_Direction this_dir)
{
    switch(last_dir)
    {
        case up:
            if      (this_dir == up) return FORWARD_COST;
            else if (this_dir == down) return BACKWARD_COST;
            else if (this_dir == left) return LEFT_COST;
            else if (this_dir == right) return RIGHT_COST;
        break;
        case down:
            if      (this_dir == up) return BACKWARD_COST;
            else if (this_dir == down) return FORWARD_COST;
            else if (this_dir == left) return RIGHT_COST;
            else if (this_dir == right) return LEFT_COST;
        break;
        case left:
            if      (this_dir == up) return RIGHT_COST;
            else if (this_dir == down) return LEFT_COST;
            else if (this_dir == left) return FORWARD_COST;
            else if (this_dir == right) return RIGHT_COST;
        break;
        case right:
            if      (this_dir == up) return LEFT_COST;
            else if (this_dir == down) return RIGHT_COST;
            else if (this_dir == left) return BACKWARD_COST;
            else if (this_dir == right) return FORWARD_COST;
        break;
        case none: return FORWARD_COST;
    }
    return std::numeric_limits<float>::max();
}


float Sokoban_Board::get_move_cost(const move &the_move)
{
    const Sokoban_Box* box = the_move.second;
    const Move_Direction &dir = the_move.first;
    //Get the square which the player is on when starting to push the box.
    const Sokoban_Box *player_start_push_box = box->get_neighbour(get_reverse_direction(dir));
    float start_push_cost = player_start_push_box->cost_to_box;
    //Get the cost of pushing the box(simply turn direction cost)
    float push_turn_cost = get_turn_direction_cost(player_start_push_box->move_dir, dir) + MOVE_COST;
    float total_cost = push_turn_cost + start_push_cost;
    if(the_move.type == Macro)
    {
        for(uint8_t i = 0; i < the_move.macro_move->size(); i++)
            total_cost += FORWARD_COST + MOVE_COST + PUSH_COST;
    }
    return total_cost;
}


std::vector<move> Sokoban_Board::get_player_moves(const std::vector<move> &box_moves)
{   //Accepts a vector containing box pushes, and return player moves
    //The board must be in the state where all the the moves will be legal in the given order.
    //The board state will be altered to be in the state after the last move.
    //Will recover the board.

    std::vector<move> player_moves;
    for(auto &the_move : box_moves)
    {
        //std::cout << the_move << std::endl;
        auto player_moves_single_push = this->get_player_moves(the_move);
        //for(auto &the_player_move : player_moves_single_push)
        //    std::cout << "\t\t" << the_player_move << std::endl;
        player_moves.insert(player_moves.end(),
            player_moves_single_push.begin(), player_moves_single_push.end());
        this->perform_move(the_move, false, true);
    }

    return player_moves;
}

std::vector<move> Sokoban_Board::get_player_moves(const move &box_move)
{   //Return player moves which needs to be done to perform the given box push
    //The board must be in a state where the given move is legal

    std::vector<move> moves;

    Sokoban_Box *cur_box = box_move.second; //the box with the barel on, will be the player box at the end.
    moves.push_back(box_move);
    cur_box = cur_box->get_neighbour(get_reverse_direction(box_move.first));
    while(cur_box->parent_node != nullptr)
    {
        moves.push_back(move(cur_box->move_dir, cur_box));
        cur_box = cur_box->parent_node;
    }
    std::reverse(moves.begin(), moves.end());
    return moves;
}

std::string Sokoban_Board::get_move_string(const std::vector<move> &moves)
{   //Return a string representing the given moves in the standard format:
    //"u, d, l, r for player moves, U, D, L, R for box pushes"
    //The given moves must be legal from this board state
    std::string move_str = "";
    //std::vector<move> box_pushes;
    for(auto the_move: moves)
    {
        unsigned char move_char = get_direction_char(the_move.first);
        //std::cout << move_char << *the_move.second << std::endl;
        if(the_move.second->is_solid())
        {
            //std::cout << *the_move.second << std::endl;
            move_str += std::toupper(move_char);
            perform_move(the_move, false, true);
            //std::cout << "moved" << std::endl;
        }
        else
            move_str += move_char;
    }
    return move_str;
}

std::string Sokoban_Board::get_reachable_map()
{ //return a string representing the "reachable" map
    std::string board_str = "";
    for(uint32_t y = 0; y < this->size_y; y++)
    {

        std::string row_str = std::to_string(y);
        row_str += " ";
        for(uint32_t x = 0; x < this->size_x; x++)
        {
            row_str += Sokoban_Board::get_reachable_str(this->board[x][y]) + " ";
        }
        board_str += row_str;
        board_str += "\n";
    }
    return board_str;
}

std::string Sokoban_Board::get_reachable_str(Sokoban_Box &box)
{
    Move_Direction dir = Move_Direction::none;
    auto sim_move = move(dir, &box);
    float cost = get_move_cost(sim_move);
    if(cost == std::numeric_limits<float>::max()) return std::string("UUUUUU");
    char formated_string[20];
    sprintf(formated_string, "%3.2f", cost);
    return std::string(formated_string);
}

void Sokoban_Board::calculate_cost_map(Sokoban_Box &box)
{
    //Calculate a wavefront map for the given box and give it to the box.
    //Quick and dirty, should probably be cleaned up at some point...
    std::vector<std::vector <float > > *cost_map = new std::vector<std::vector <float > >;
    for(uint32_t x = 0; x < this->size_x; x++)
    {
        std::vector<float> row_vec;
        cost_map->push_back(row_vec);
        for(uint32_t y = 0; y < this->size_y; y++)
        {
            cost_map->back().push_back(std::numeric_limits<float>::max());
        }
    }
    (*cost_map)[box.pos.x_pos][box.pos.y_pos] = 0;
    auto expand_points_this = new std::vector<Sokoban_Box *>; //points to expand from in this run
    auto expand_points_next = new std::vector<Sokoban_Box *>; //points to expand from in the next run
    expand_points_this->push_back(&box);
    while(expand_points_this->size())
    {
        for(auto this_point : *expand_points_this)
        {
            if(calculate_cost_map_helper(this_point, Move_Direction::up, cost_map))
                expand_points_next->push_back(this_point->get_neighbour(Move_Direction::up));
            if(calculate_cost_map_helper(this_point, Move_Direction::down, cost_map))
                expand_points_next->push_back(this_point->get_neighbour(Move_Direction::down));
            if(calculate_cost_map_helper(this_point, Move_Direction::left, cost_map))
                expand_points_next->push_back(this_point->get_neighbour(Move_Direction::left));
            if(calculate_cost_map_helper(this_point, Move_Direction::right, cost_map))
                expand_points_next->push_back(this_point->get_neighbour(Move_Direction::right));
        }
        expand_points_this->clear();
        std::swap(expand_points_this, expand_points_next);
    }
    delete expand_points_this;
    delete expand_points_next;
    box.set_cost_map(cost_map);
}

bool Sokoban_Board::calculate_cost_map_helper(const Sokoban_Box *this_box, Move_Direction dir,
    std::vector<std::vector <float > > *cost_map)
{
    Sokoban_Box *nb = this_box->get_neighbour(dir);
    if(this_box->is_moveable(dir) &&
    (*cost_map)[this_box->pos.x_pos][this_box->pos.y_pos] + 1 <
    (*cost_map)[nb->pos.x_pos][nb->pos.y_pos] )
    {
        (*cost_map)[nb->pos.x_pos][nb->pos.y_pos] =
            (*cost_map)[this_box->pos.x_pos][this_box->pos.y_pos] + 1;
        return true;
    }
    return false;
}

/*
void Sokoban_Board::calc_reachable(__attribute__((unused)) Move_Direction last_move_dir)
{
    this->upper_left_reachable = this->player_box;
    for(uint32_t x = 0; x < this->size_x; x++)
        for(uint32_t y = 0; y < this->size_y; y++)
            this->board[x][y].cost_to_box = std::numeric_limits<float>::max();

    this->board[player_box->pos.x_pos][player_box->pos.y_pos].cost_to_box = 1;
    if(!player_box->nb_up->is_solid())      this->calc_reachable_rec(player_box->nb_up);
    if(!player_box->nb_down->is_solid())    this->calc_reachable_rec(player_box->nb_down);
    if(!player_box->nb_left->is_solid())    this->calc_reachable_rec(player_box->nb_left);
    if(!player_box->nb_right->is_solid())   this->calc_reachable_rec(player_box->nb_right);
}

void Sokoban_Board::calc_reachable_rec(Sokoban_Box *box)
{
    if(box->pos < this->upper_left_reachable->pos) upper_left_reachable = box;
    this->board[box->pos.x_pos][box->pos.y_pos].cost_to_box = 1;
    if(!box->nb_up->is_solid() && !this->is_reachable(box->nb_up))
        this->calc_reachable_rec(box->nb_up);
    if(!box->nb_down->is_solid() && !this->is_reachable(box->nb_down))
        this->calc_reachable_rec(box->nb_down);
    if(!box->nb_left->is_solid() && !this->is_reachable(box->nb_left))
        this->calc_reachable_rec(box->nb_left);
    if(!box->nb_right->is_solid() && !this->is_reachable(box->nb_right))
        this->calc_reachable_rec(box->nb_right);

}
*/
