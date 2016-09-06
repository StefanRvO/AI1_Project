#pragma once
#include <cstdint>
#include <vector>
#include <utility>


//A box defining a field on the Sokoban board
struct Position
{
    uint32_t x_pos;
    uint32_t y_pos;
};

enum Box_Type
{
    Box, //A box which is not yet placed upon a goal $
    Goal_Box, //A box placed upon a goal *
    Goal, //A goal to place boxes on .
    Wall, //Wall #
    Player, //Player. @
    Player_On_Goal, //Player standing on a goal square. +
    Free, //Free space. (Space)/-/_
    Free_Searched, //Free space, but searched. Used when finding moves.
    Goal_Searched, //Goal, but searched. Used when finding moves.
};

enum Move_Direction
{
    up,
    down,
    left,
    right,
};
class Sokoban_Box;


typedef std::pair<Move_Direction, Sokoban_Box *> move;


class Sokoban_Box
{
    private:
    public:
        //This defines the box's neighbours.
        Sokoban_Box *nb_up = nullptr;
        Sokoban_Box *nb_down = nullptr;
        Sokoban_Box *nb_left = nullptr;
        Sokoban_Box *nb_right = nullptr;


        //Set the neighbours. Should be called after a move as this, for performance
        //Reasons, is not done automatically
        void set_neighbours(Sokoban_Box *_nb_up, Sokoban_Box *_nb_down,
            Sokoban_Box *_nb_left, Sokoban_Box *_nb_right);
        Box_Type type;
        Position pos;
        //Change which type the box is.
        void change_type(Box_Type new_type);

        //Move the box.
        //The box itself is actually not moved, instead, the type of the origin
        //And destination box is simply change accordingly.
        //This also means that we don't need to change the neighbour pointers or position when moving.
        //Only types need to be change.
        //Be aware, that if a higher level datastructure keeps track of the boxes, these
        //may need to be adjusted according to the move.
        //if the player box is given, the player will be moved to the position it should push
        //the box from. No check if performed to test if this is valid
        void move(Move_Direction dir, Sokoban_Box *player_box = nullptr, bool reserse = false);

        void change_types_in_move(Sokoban_Box &old_box, Sokoban_Box &new_box);
        bool is_moveable(Move_Direction dir);

        Sokoban_Box(Box_Type _type, Position _pos);
        Sokoban_Box() {};
};
