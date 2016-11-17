#pragma once
#include <cstdint>
#include <vector>
#include <utility>
#include <ostream>
#include "Sokoban_Move.hpp"
#include "enums.hpp"
#include <list>
//A box defining a field on the Sokoban board


class Sokoban_Move;
typedef Sokoban_Move move;

struct Position
{
    uint32_t x_pos;
    uint32_t y_pos;
};

//Check vertical first, then horisontal.
bool operator<(const Position &pos1, const Position &pos2);


unsigned char get_direction_char(Move_Direction &dir);
Move_Direction get_reverse_direction(Move_Direction dir);


class Sokoban_Box;


std::ostream& operator<<(std::ostream& os, const move& the_move);

class Sokoban_Box
{
    private:
    public:
        int64_t _deadlocked = 0xFFFFFFFFFFFF; //s64 because we parse a random number into the, and it should be unreasonable to get the same two times in a row.
        //negative means not deadlocked


        //This defines the box's neighbours.
        Sokoban_Box *nb_up = nullptr;
        Sokoban_Box *nb_down = nullptr;
        Sokoban_Box *nb_left = nullptr;
        Sokoban_Box *nb_right = nullptr;
        //Used for computing a move cost map
        float cost_to_box = 0;
        bool closed = false;
        Sokoban_Box *parent_node = nullptr;
        Move_Direction move_dir = Move_Direction::up; //Direction moved last to reach this box from the player

        std::vector< std::vector < float > > *cost_map = nullptr; //Map of cost to reach each individual box on the map.

        Tunnel_Type tunnel_type = None;
        Orientation tunnel_orientation = Horizontal;
        std::vector<Sokoban_Box *> tunnel_members;
         //List of macro moves. Indexed by the direction the box comes from
         //There can only be max two macro moves per field, but this is easier,
         //and don't cause much overhead anyway.
        std::list<Sokoban_Move> macro_move[4];

        //Set the neighbours. Should be called after a move as this, for performance
        //Reasons, is not done automatically
        void set_neighbours(Sokoban_Box *_nb_up, Sokoban_Box *_nb_down,
            Sokoban_Box *_nb_left, Sokoban_Box *_nb_right);
        Box_Type type;
        Position pos;
        //Change which type the box is.
        void change_type(Box_Type new_type);
        std::vector<Sokoban_Box *> freeze_group;
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
        void set_cost_map(std::vector< std::vector <float > > *_cost_map){ this->cost_map = _cost_map; }
        float get_cost_to_box(Sokoban_Box &box);
        static void move(Sokoban_Box * &move_box, Sokoban_Box * &player_box, Move_Direction dir, bool reverse = false);

        static void change_types_in_move(Sokoban_Box &old_box, Sokoban_Box &new_box);
        bool is_moveable(Move_Direction dir) const;
        bool is_deadlocked();
        bool is_pullable(Move_Direction dir);
        bool is_freeze_deadlocked(int64_t rand_num);
        void propegate_deadlock(int64_t rand_num, const Move_Direction *no_check_dir = nullptr);
        bool is_freeze_deadlocked_helper(int64_t rand_num, Move_Direction dir1, Move_Direction dir2,  std::vector<Sokoban_Box *> &grp);
        bool is_freeze_deadlocked(int64_t rand_num, const Move_Direction *no_check_dir, std::vector<Sokoban_Box *> &grp);

        friend std::ostream& operator<<(std::ostream& os, const Sokoban_Box& box)
        {
            os << "(" << box.pos.x_pos << "," << box.pos.y_pos << ")";
            return os;
        }
        Sokoban_Box *get_neighbour(Move_Direction dir) const;
        ~Sokoban_Box();
        uint8_t get_moveable_count() const;
        Sokoban_Box(Box_Type _type, Position _pos);
        Sokoban_Box() {};
        bool is_solid() const;
        bool is_in_deadlock_zone() const;
        bool is_tunnel(bool deadzone = true) const;
        bool is_tunnel(Orientation orientation, bool deadzone = true) const;
        bool operator() (Sokoban_Box *first, Sokoban_Box *second)
        {
            if(first->cost_to_box == second->cost_to_box)
            { //Compare pointer value, we don't allow equal elements.
                return (void *)first < (void *)second;
            }
            return first->cost_to_box < second->cost_to_box;
        }
        void insert_player();
        void remove_player();
        void insert_box();
        void remove_box();

};
