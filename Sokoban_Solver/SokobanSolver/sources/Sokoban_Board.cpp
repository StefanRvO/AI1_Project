#include <boost/algorithm/string.hpp> //boost::split and boost::is_any_of
#include "Sokoban_Board.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <random>
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

char Sokoban_Board::get_box_char(Box_Type type)
{
    switch(type)
    {
        case Wall:            return '#';
        case Goal_Box:        return '*';
        case Goal:            return '.';
        case Box:             return '$';
        case DeadLock_Zone_Player:
        case Player:          return '@';
        case Player_On_Goal:  return '+';
        case DeadLock_Zone_Free:
        case Free:            return ' ';
        default:
        throw std::runtime_error(std::string("Trying to convert invalid type to char.\nThis should not happen!"));
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
}


//Copy constructor
Sokoban_Board::Sokoban_Board(Sokoban_Board &_board)
: rand_gen(_board.rand_gen), gen(_board.gen)
{
    this->board = _board.board;
    this->size_x = _board.size_x;
    this->size_y = _board.size_y;
    //Create_neighbour pointers
    this->populate_neighbours();
}

Sokoban_Board::Sokoban_Board(std::string &board_str)
: rand_gen(1, 0x0FFFFFFFFFFFFFFF),  gen(this->rd())

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
            row_str += Sokoban_Board::get_box_char(this->board[x][y].type);
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
}

std::vector<move> Sokoban_Board::find_possible_moves()
{   //Recursive move finder algorithm. This is probably pretty slow, so we should
    //maybe try to figure out a faster way..?
    std::vector<Sokoban_Box *> searched_fields;
    searched_fields.reserve(this->size_x * this->size_y);
    //Search all around the player
    std::vector<move> moves;
    moves.reserve(10);
    Sokoban_Board::find_possible_moves_rec(Move_Direction::up, this->player_box->nb_up, searched_fields, moves);
    Sokoban_Board::find_possible_moves_rec(Move_Direction::down, this->player_box->nb_down, searched_fields, moves);
    Sokoban_Board::find_possible_moves_rec(Move_Direction::left, this->player_box->nb_left, searched_fields, moves);
    Sokoban_Board::find_possible_moves_rec(Move_Direction::right, this->player_box->nb_right, searched_fields, moves);

    //Clear searched types
    for(auto &box : searched_fields)
    {
        if(box->type == Free_Searched)                      box->type = Free;
        else if(box->type == Goal_Searched)                 box->type = Goal;
        else if(box->type == DeadLock_Zone_Free_Searched)   box->type = DeadLock_Zone_Free;
    }
    return moves;
}

void Sokoban_Board::find_possible_moves_rec(Move_Direction dir, Sokoban_Box *search_box, std::vector<Sokoban_Box *> &searched_fields, std::vector<move> &moves)
{
    Box_Type &this_type = search_box->type;
    switch(this_type)
    {
        case Free_Searched:
        case Goal_Searched:
        case DeadLock_Zone_Free_Searched:
        case Wall:
        case Player:
        case DeadLock_Zone_Player:
        case Player_On_Goal:
            return;
        case Free:
            search_box->type = Free_Searched;
            searched_fields.push_back(search_box);
            break;
        case DeadLock_Zone_Free:
            search_box->type = DeadLock_Zone_Free_Searched;
            searched_fields.push_back(search_box);
            break;
        case Goal:
            search_box->type = Goal_Searched;
            searched_fields.push_back(search_box);
            break;
        case Box:
        case Goal_Box:
            if(search_box->is_moveable(dir)) moves.push_back(move(dir, search_box));
            return;
        default:
            std::cout << this_type << std::endl;
            assert(false);
    }
    //Search around search_box, but not in the direction we came from.
    if(dir != down)
        Sokoban_Board::find_possible_moves_rec(Move_Direction::up, search_box->nb_up, searched_fields, moves);
    if(dir != up)
        Sokoban_Board::find_possible_moves_rec(Move_Direction::down, search_box->nb_down, searched_fields, moves);
    if(dir != right)
        Sokoban_Board::find_possible_moves_rec(Move_Direction::left, search_box->nb_left, searched_fields, moves);
    if(dir != left)
        Sokoban_Board::find_possible_moves_rec(Move_Direction::right, search_box->nb_right, searched_fields, moves);
}

void Sokoban_Board::perform_move(move the_move, bool reverse)
{
    Sokoban_Box *start_pos  = nullptr;
    Sokoban_Box *end_pos    = nullptr;

    if(reverse == false)
    {
        start_pos = the_move.second;
        Sokoban_Box::move(the_move.second, this->player_box, the_move.first, reverse);
        end_pos = the_move.second;
    }
    else
    {
        //Perform move
        start_pos = the_move.second->get_neighbour(the_move.first);
        end_pos = the_move.second;
        Sokoban_Box::move(the_move.second, this->player_box, the_move.first, reverse);
    }
    #ifndef NDEBUG
    auto start_size = this->board_boxes.size();
    #endif
    this->board_boxes.erase(start_pos);
    this->board_boxes.insert(std::pair<Sokoban_Box *,Sokoban_Box *>(end_pos, end_pos));
    assert(start_size == this->board_boxes.size());
}

int32_t Sokoban_Board::get_heuristic()
{   //Heuristic function. Very simple. Should probably be improved.
    //Give an estimate of the number of remaining moves.
    //This is calculated as the manhattan distance(sum of horisontal and vertical distance)
    //We also check for (very simple) deadlocks. We return a negative number if a deadlock
    //is detected.
    //Generate random number for deadlock detection
    //static uint32_t calls = 0;
    //if(calls++ % 1000 == 0) std::cout << calls << std::endl;

    int64_t rand_num = rand_gen(gen);
    int32_t h_cost = 0;
    for(auto &box_pair : this->board_boxes)
    {
        auto &box = box_pair.first;
        if(box->is_freeze_deadlocked(rand_num))
            if(box->type != Goal_Box)
                return -1;
        if(box->type == Goal_Box) continue;
        uint32_t min_distance = 0xFFFFFF;
        for(auto &goal : this->goals)
        {
//            std::cout << "GOAL: " << goal->pos.x_pos << " " <<
//                goal->pos.y_pos << std::endl <<std::endl;

            if(box == goal)
            {
                std::cout << "!!" << std::endl;
                min_distance = 0;
                break;
            }
            uint32_t dist = abs(box->pos.x_pos - goal->pos.x_pos) +
                abs(box->pos.y_pos - goal->pos.y_pos);
            min_distance = std::min(min_distance, dist);
        }
        h_cost += min_distance;
    }
    return h_cost;
}
