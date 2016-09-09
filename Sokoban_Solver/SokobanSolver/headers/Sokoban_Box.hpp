#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <ostream>

//A box defining a field on the Sokoban board
struct Position
{
    uint32_t x_pos;
    uint32_t y_pos;
};

enum Box_Type : uint8_t
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
    DeadLock_Zone_Free, //Blocks which, if a box would be placed there, it would cause a deadlock.
    DeadLock_Zone_Free_Searched,
    DeadLock_Zone_Player,
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
        int64_t _deadlocked = 0; //s64 because we parse a random number into the, and it should be unreasonable to get the same two times in a row.
        //negative means not deadlocked


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

        //Move the box given by move_box.
        //The box itself is actually not moved, instead, the type of the origin
        //and destination box is simply change accordingly.
        //This also means that we don't need to change the neighbour pointers or position when moving.
        //Only types need to be change.
        //The player will be moved to the position it should push
        //the box from. No check if performed to test if this is valid.

        //The move_box and player_box are given as pointer references.
        //During the call, these pointers will be changed to point to the new position
        //for the player and moved box.
        static void move(Sokoban_Box * &move_box, Sokoban_Box * &player_box, Move_Direction dir, bool reverse = false);

        static void change_types_in_move(Sokoban_Box &old_box, Sokoban_Box &new_box);
        bool is_moveable(Move_Direction dir);
        bool is_deadlocked();
        bool is_pullable(Move_Direction dir);
        bool is_freeze_deadlocked(int64_t rand_num, const Move_Direction *no_check_dir = nullptr);
        void propegate_deadlock(int64_t rand_num, const Move_Direction *no_check_dir = nullptr);
        friend std::ostream& operator<<(std::ostream& os, const Sokoban_Box& box)
        {
            os << "(" << box.pos.x_pos << "," << box.pos.y_pos << ")";
            return os;
        }
        Sokoban_Box *get_neighbour(Move_Direction dir);

        Sokoban_Box(Box_Type _type, Position _pos);
        Sokoban_Box() {};
};
