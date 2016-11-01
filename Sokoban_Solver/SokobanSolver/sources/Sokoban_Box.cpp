#include "Sokoban_Box.hpp"
#include <cassert>
#include <iostream>

bool operator<(const Position &pos1, const Position &pos2)
{
    if(pos1.x_pos < pos2.x_pos)
        return true;
    else if(pos1.x_pos > pos2.x_pos)
        return false;
    if(pos1.y_pos < pos2.y_pos)
        return true;
    else
        return false;

}

Move_Direction get_reverse_direction(Move_Direction dir)
{
    switch (dir) {
        case up: return down;
        case down: return up;
        case left: return right;
        case right: return left;

    }
    assert(false);
    return left;
}



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
            else
            {
                std::cout << old_box << "\t" << new_box << std::endl;
                assert(false);
            }
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
    //First move the player, as the box may be moved to the player position.
    if(player_box != nullptr and player_box != new_player_pos)
        Sokoban_Box::change_types_in_move(*player_box, *new_player_pos);
    if(reverse)
        Sokoban_Box::change_types_in_move(*new_pos, *move_box);
    else
        Sokoban_Box::change_types_in_move(*move_box, *new_pos);

    if(!reverse) move_box = new_pos;
    player_box = new_player_pos;
    //If we aren't reversing, and want to move the player, move the player one up now!
    if(!reverse && player_box != nullptr)
    {
        Sokoban_Box::change_types_in_move(*player_box, *player_box->get_neighbour(dir));
        player_box = player_box->get_neighbour(dir);
    }
}


bool Sokoban_Box::is_moveable(Move_Direction dir)
{
    switch(this->get_neighbour(dir)->type)
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
    return this;
}

bool Sokoban_Box::is_freeze_deadlocked_helper(int64_t rand_num, Move_Direction dir1, Move_Direction dir2)
{
    auto nb_1 = this->get_neighbour(dir1);
    auto nb_2 = this->get_neighbour(dir2);
    auto &type1 = nb_1->type;
    auto &type2 = nb_2->type;

    if(type1 == Wall or type2 == Wall) return true;
    if( (type1 == DeadLock_Zone_Player or type1 == DeadLock_Zone_Free) and
        (type2 == DeadLock_Zone_Player or type2 == DeadLock_Zone_Free))
        return true;
    if(type1 == Goal_Box or type1 == Box)
        if(nb_1->is_freeze_deadlocked(rand_num, &dir1))
            return true;
    if(type2 == Goal_Box or type2 == Box)
        if(nb_2->is_freeze_deadlocked(rand_num, &dir2))
            return true;
    return false;
}

bool Sokoban_Box::is_freeze_deadlocked(int64_t rand_num, const Move_Direction *no_check_dir)
{
    //std::cout << "visted: " << *this << std::endl;
    if(this->_deadlocked == rand_num) return true;
    if(this->_deadlocked == -rand_num) return false;
    if(no_check_dir)
    {
        if(*no_check_dir == left or *no_check_dir == right)
        {
            if(this->is_freeze_deadlocked_helper(rand_num, up, down))
                return true;
            return false;
        }
        else
        {
            if(this->is_freeze_deadlocked_helper(rand_num, left, right))
                return true;
            return false;
        }
    }
    else
    {
        if(this->is_freeze_deadlocked_helper(rand_num, up, down) and
           this->is_freeze_deadlocked_helper(rand_num, left, right))
        {
              this->_deadlocked = rand_num;
              return true;
        }
        this->_deadlocked = -rand_num;
        return false;
    }
}

void Sokoban_Box::propegate_deadlock(int64_t rand_num, const Move_Direction *no_check_dir)
{
    if(this->_deadlocked == rand_num or this->_deadlocked == -rand_num) return;
    for(uint8_t _dir = Move_Direction::up; _dir <= Move_Direction::right; _dir++)
    {
        const Move_Direction &dir = (Move_Direction)_dir;
        if(no_check_dir and *no_check_dir == dir) continue;
        switch (this->get_neighbour(dir)->type)
        {
            case Box: case Goal_Box:
                this->propegate_deadlock(rand_num, &dir);
            default:
                break;
        }
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
bool Sokoban_Box::is_solid() const
{
    switch(this->type)
    {
        case Box:
        case Goal_Box:
        case Wall:
            return true;
        default:
            return false;
    }
}
