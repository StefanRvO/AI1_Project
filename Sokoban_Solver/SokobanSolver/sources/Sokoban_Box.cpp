#include "Sokoban_Box.hpp"
#include <cassert>
#include <iostream>
Sokoban_Box::Sokoban_Box(Box_Type _type, Position _pos)
{
    this->type = _type;
    this->pos = _pos;
}

void Sokoban_Box::change_type(Box_Type new_type)
{
    this->type = new_type;
}

void Sokoban_Box::set_neighbours(Sokoban_Box *_nb_up, Sokoban_Box *_nb_down,
    Sokoban_Box *_nb_left, Sokoban_Box *_nb_right)
{
    this->nb_up = _nb_up;
    this->nb_down = _nb_down;
    this->nb_left = _nb_left;
    this->nb_right = _nb_right;
}


void Sokoban_Box::change_types_in_move(Sokoban_Box &old_box, Sokoban_Box &new_box)
{   //Change the types of the old_box and new_box box, as if the player or box at
    //old pos was moved to new pos.

    switch(old_box.type)
    {
        case Box:
            old_box.change_type(Box_Type::Free);
            if(new_box.type == Goal) new_box.change_type(Box_Type::Goal_Box);
            else if(new_box.type == Free) new_box.change_type(Box_Type::Box);
            else assert(false);
            break;
        case Goal_Box:
            old_box.change_type(Box_Type::Goal);
            if(new_box.type == Goal) new_box.change_type(Box_Type::Goal_Box);
            else if(new_box.type == Free) new_box.change_type(Box_Type::Box);
            else assert(false);
            break;
        case Player:
            old_box.change_type(Box_Type::Free);
            if(new_box.type == Goal) new_box.change_type(Box_Type::Player_On_Goal);
            else if(new_box.type == Free) new_box.change_type(Box_Type::Player);
            else assert(false);
            break;
        case Player_On_Goal:
            old_box.change_type(Box_Type::Goal);
            if(new_box.type == Goal) new_box.change_type(Box_Type::Player_On_Goal);
            else if(new_box.type == Free) new_box.change_type(Box_Type::Player);
            else assert(false);
            break;
        default:
            assert(false);
    }
}


void Sokoban_Box::move( Move_Direction dir, Sokoban_Box *player_box, bool reverse)
{   Sokoban_Box *new_pos = nullptr;
    Sokoban_Box *new_player_pos = nullptr;
    if(reverse == true)
    {
        switch(dir)
        {
            case Move_Direction::up:
                new_pos = this->nb_down;
                new_player_pos = this->nb_down->nb_down;
                break;
            case Move_Direction::down:
                new_pos = this->nb_up;
                new_player_pos = this->nb_up->nb_up;
                break;
            case Move_Direction::left:
                new_pos = this->nb_right;
                new_player_pos = this->nb_right->nb_right;
                break;
            case Move_Direction::right:
                new_pos = this->nb_left;
                new_player_pos = this->nb_left->nb_left;
                break;
            default:
                assert(false);
        }
    }
    else
    {
        switch(dir)
        {
            case Move_Direction::up:
                new_pos = this->nb_up;
                new_player_pos = this->nb_down;
                break;
            case Move_Direction::down:
                new_pos = this->nb_down;
                new_player_pos = this->nb_up;
                break;
            case Move_Direction::left:
                new_pos = this->nb_left;
                new_player_pos = this->nb_right;
                break;
            case Move_Direction::right:
                new_pos = this->nb_right;
                new_player_pos = this->nb_left;
                break;
            default:
                assert(false);
        }
    }
    if(player_box)
        Sokoban_Box::change_types_in_move(*player_box, *new_player_pos);

    Sokoban_Box::change_types_in_move(*this, *new_pos);
}


bool Sokoban_Box::is_moveable(Move_Direction dir)
{
    Sokoban_Box *move_dir_box = nullptr;
    switch(dir)
    {
        case up:
            move_dir_box = this->nb_up;
            break;
        case down:
            move_dir_box = this->nb_down;
            break;
        case left:
            move_dir_box = this->nb_left;
            break;
        case right:
            move_dir_box = this->nb_right;
            break;
        default:
            std::cout << dir << std::endl;
            assert(false);
    }
    
    switch(move_dir_box->type)
    {
        case Box:
        case Goal_Box:
        case Wall:
            return false;
        default:
            return true;
    }

}
