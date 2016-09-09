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
            old_box.change_type(Free);
            if(new_box.type == Goal) new_box.change_type(Goal_Box);
            else if(new_box.type == Free) new_box.change_type(Box);
            else assert(false);
            break;
        case Goal_Box:
            old_box.change_type(Goal);
            if(new_box.type == Goal) new_box.change_type(Goal_Box);
            else if(new_box.type == Free) new_box.change_type(Box);
            else assert(false);
            break;
        case Player:
            old_box.change_type(Free);
            if(new_box.type == Goal) new_box.change_type(Player_On_Goal);
            else if(new_box.type == Free) new_box.change_type(Player);
            else if(new_box.type == DeadLock_Zone_Free) new_box.change_type(DeadLock_Zone_Player);
            else assert(false);
            break;
        case Player_On_Goal:
            old_box.change_type(Goal);
            if(new_box.type == Goal) new_box.change_type(Player_On_Goal);
            else if(new_box.type == Free) new_box.change_type(Player);
            else if(new_box.type == DeadLock_Zone_Free) new_box.change_type(DeadLock_Zone_Player);
            else assert(false);
            break;
        case DeadLock_Zone_Player:
            old_box.change_type(DeadLock_Zone_Free);
            if(new_box.type == Goal) new_box.change_type(Player_On_Goal);
            else if(new_box.type == Free) new_box.change_type(Player);
            else if(new_box.type == DeadLock_Zone_Free) new_box.change_type(DeadLock_Zone_Player);
            else assert(false);
            break;
        default:
            std::cout << old_box.type << std::endl;
            std::cout << old_box.pos.y_pos << " " << old_box.pos.x_pos << std::endl << std::endl;
            assert(false);
    }
}


void Sokoban_Box::move(Sokoban_Box * &move_box, Sokoban_Box * &player_box, Move_Direction dir, bool reverse)
{   Sokoban_Box *new_pos = nullptr;
    Sokoban_Box *new_player_pos = nullptr;
    switch(dir)
    {
        case Move_Direction::up:
            new_pos = move_box->nb_up;
            new_player_pos = move_box->nb_down;
            break;
        case Move_Direction::down:
            new_pos = move_box->nb_down;
            new_player_pos = move_box->nb_up;
            break;
        case Move_Direction::left:
            new_pos = move_box->nb_left;
            new_player_pos = move_box->nb_right;
            break;
        case Move_Direction::right:
            new_pos = move_box->nb_right;
            new_player_pos = move_box->nb_left;
            break;
        default:
            assert(false);
    }
    if(player_box != nullptr and player_box != new_player_pos)
        Sokoban_Box::change_types_in_move(*player_box, *new_player_pos);
    if(reverse)
        Sokoban_Box::change_types_in_move(*new_pos, *move_box);
    else
        Sokoban_Box::change_types_in_move(*move_box, *new_pos);

    if(!reverse) move_box = new_pos;
    player_box = new_player_pos;

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
        case DeadLock_Zone_Free:
        case DeadLock_Zone_Player:
        case DeadLock_Zone_Free_Searched:
            return false;
        default:
            return true;
    }

}

bool Sokoban_Box::is_deadlocked()
{
    //the box is deadlocked if to adjecent neighbours are
    //Walls
    if(this->type == DeadLock_Zone_Free or this->type == DeadLock_Zone_Player) return true;
    return false;
}

Sokoban_Box *Sokoban_Box::get_neighbour(Move_Direction dir)
{   //Return the pointer to the neighbour in the given direction
    switch(dir)
    {
        case up: return this->nb_up;
        case down: return this->nb_down;
        case left: return this->nb_left;
        case right: return this->nb_right;
        default:
            assert(false);
    }
}

bool Sokoban_Box::is_pullable(Move_Direction dir)
{
    switch(this->get_neighbour(dir)->type)
    {
        case Box: case Goal_Box: case Wall:
            return false;
        default:
            break;
    }
    switch(this->get_neighbour(dir)->get_neighbour(dir)->type)
    {
        case Box: case Goal_Box: case Wall:
            return false;
        default:
            return true;
    }
}
