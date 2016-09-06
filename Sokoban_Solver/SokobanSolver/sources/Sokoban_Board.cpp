#include <boost/algorithm/string.hpp> //boost::split and boost::is_any_of
#include "Sokoban_Board.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

Box_Type Sokoban_Board::parse_char(char chr)
{ //Parse a single character from the board string.
  //Return a Box_Type
    switch(chr)
    {
        case '#': return Box_Type::Wall;
        case '*': return Box_Type::Goal_Box;
        case '.': return Box_Type::Goal;
        case '$': return Box_Type::Box;
        case '@': return Box_Type::Player;
        case '+': return Box_Type::Player_On_Goal;
        case ' ':
        case '-':
        case '_': return Box_Type::Free;
        default:
        throw std::runtime_error(std::string("Trying to parse malformated string!\nProblematic character: ") + chr);
    }
}

char Sokoban_Board::get_box_char(Box_Type type)
{
    switch(type)
    {
        case Box_Type::Wall:            return '#';
        case Box_Type::Goal_Box:        return '*';
        case Box_Type::Goal:            return '.';
        case Box_Type::Box:             return '$';
        case Box_Type::Player:          return '@';
        case Box_Type::Player_On_Goal:  return '+';
        case Box_Type::Free:            return ' ';
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
{
    this->board = _board.board;
    this->size_x = _board.size_x;
    this->size_y = _board.size_y;
    //Create_neighbour pointers
    this->populate_neighbours();
}

Sokoban_Board::Sokoban_Board(std::string &board_str)
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
                collumn.push_back(Sokoban_Box(Box_Type::Wall, {x, y}));
            else
                collumn.push_back(board_vec[y][x]);
        }
    }
    this->populate_neighbours();
}

std::string Sokoban_Board::get_board_str()
{
    std::string board_str = "";
    for(uint32_t y = 0; y < this->size_y; y++)
    {
        std::string row_str = "";
        for(uint32_t x = 0; x < this->size_x; x++)
        {
            row_str += Sokoban_Board::get_box_char(this->board[x][y].type);
        }
        board_str += row_str;
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
            if(this->board[x][y].type == Box_Type::Player or this->board[x][y].type == Box_Type::Player_On_Goal)
                this->player_box = &this->board[x][y];
        }
    }
}

std::vector<move> Sokoban_Board::find_possible_moves()
{   //Recursive move finder algorithm. This is probably pretty slow, so we should
    //maybe try to figure out a faster way..?
    std::vector<Sokoban_Box *> searched_fields;
    //Search all around the player
    std::vector<move> moves;
    std::cout << "(" << this->player_box->pos.x_pos << ",";
    std::cout << this->player_box->pos.y_pos << ")" << std::endl;

    auto moves_up =     Sokoban_Board::find_possible_moves_rec(Move_Direction::up, this->player_box->nb_up, searched_fields);
    auto moves_down =   Sokoban_Board::find_possible_moves_rec(Move_Direction::down, this->player_box->nb_down, searched_fields);
    auto moves_left =   Sokoban_Board::find_possible_moves_rec(Move_Direction::left, this->player_box->nb_left, searched_fields);
    auto moves_right =  Sokoban_Board::find_possible_moves_rec(Move_Direction::right, this->player_box->nb_right, searched_fields);
    moves.insert( moves.end(), std::make_move_iterator(moves_up.begin()), std::make_move_iterator(moves_up.end()));
    moves.insert( moves.end(), std::make_move_iterator(moves_down.begin()), std::make_move_iterator(moves_down.end()));
    moves.insert( moves.end(), std::make_move_iterator(moves_left.begin()), std::make_move_iterator(moves_left.end()));
    moves.insert( moves.end(), std::make_move_iterator(moves_right.begin()), std::make_move_iterator(moves_right.end()));

    //Clear searched types
    for(auto &box : searched_fields)
    {
        if(box->type == Free_Searched)       box->type = Free;
        else if(box->type == Goal_Searched)  box->type = Goal;
    }
    std::cout << std::endl << std::endl;
    return moves;
}

std::vector<move> Sokoban_Board::find_possible_moves_rec(Move_Direction dir, Sokoban_Box *search_box, std::vector<Sokoban_Box *> &searched_fields)
{
    std::vector<move> moves;
    Box_Type &this_type = search_box->type;
    switch(this_type)
    {
        case Free_Searched:
        case Goal_Searched:
        case Wall:
        case Player:
        case Player_On_Goal:
            return moves;
        case Free:
            search_box->type = Free_Searched;
            searched_fields.push_back(search_box);
            break;
        case Goal:
            search_box->type = Goal_Searched;
            searched_fields.push_back(search_box);
            break;
        case Box:
        case Goal_Box:
            if(search_box->is_moveable(dir)) moves.push_back(move(dir, search_box));
            return moves;
        default:
            assert(false);
    }
    //Search around search_box, but not in the direction we came from.
    if(dir != down)
    {
        auto moves_up = Sokoban_Board::find_possible_moves_rec(Move_Direction::up, search_box->nb_up, searched_fields);
        moves.insert( moves.end(), std::make_move_iterator(moves_up.begin()), std::make_move_iterator(moves_up.end()));
    }
    if(dir != up)
    {
        auto moves_down =   Sokoban_Board::find_possible_moves_rec(Move_Direction::down, search_box->nb_down, searched_fields);
        moves.insert( moves.end(), std::make_move_iterator(moves_down.begin()), std::make_move_iterator(moves_down.end()));
    }
    if(dir != right)
    {
        auto moves_left =  Sokoban_Board::find_possible_moves_rec(Move_Direction::left, search_box->nb_left, searched_fields);
        moves.insert( moves.end(), std::make_move_iterator(moves_left.begin()), std::make_move_iterator(moves_left.end()));
    }
    if(dir != left)
    {
        auto moves_right =  Sokoban_Board::find_possible_moves_rec(Move_Direction::right, search_box->nb_right, searched_fields);
        moves.insert( moves.end(), std::make_move_iterator(moves_right.begin()), std::make_move_iterator(moves_right.end()));
    }
    return moves;
}
