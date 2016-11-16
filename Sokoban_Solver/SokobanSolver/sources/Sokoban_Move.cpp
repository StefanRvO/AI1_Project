#include "Sokoban_Move.hpp"
Sokoban_Move::Sokoban_Move(Move_Direction _first, Sokoban_Box *_second, bool ignore_macro)
:first(_first), second(_second)
{
    if(this->second->get_neighbour(this->first)->macro_move[(int)_first].size() == 0 || ignore_macro)
        return;
    this->macro_move = &this->second->get_neighbour(this->first)->macro_move[(int)_first];
    //check if the macro move is valid, i.e., that all fields are clear.
    for(auto &the_move : *this->macro_move)
    {
        if(the_move.second->get_neighbour(the_move.first)->is_solid() ||
            the_move.second->get_neighbour(the_move.first)->is_in_deadlock_zone())
        {
            this->type = Invalid;
            return;
        }
    }
    this->type = Macro;
}
