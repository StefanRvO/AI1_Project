#pragma once
//This class tries to optimise the solving by finding tunnel macros.
//Boxes being pushed inside a tunnel should be macro-moved to the other side of the
//Tunnel, as there is no more optimal way to push them than to push them straight through.
//There are two kind of tunnels: One way tunnels and two way tunnels.
//two way tunnels are tunnels which are not the only connection between two parts of an map,
//e.g., the player can push the box into the tunnel from one side, and afterwards push
//It out again from the other side, thus allowing for "parking" of stones.
//One way tunnels are the only connection between two parts of an map.
//This macro creator should be run after static deadlock detection have been performed, or its results may be less usefull(?).
#include "Sokoban_Board.hpp"

class TunnelMacroCreator
{
    private:

    Sokoban_Board *the_board;
    void compute_macros_internal();
    void mark_candidates();
    std::vector<Sokoban_Box *> find_entrances();
    Tunnel_Type set_tunnel_type(Sokoban_Box *box);
    void compute_tunnel_members(Sokoban_Box *entrance);
    void create_two_way_macro(Sokoban_Box *entrance);
    void create_one_way_macro(Sokoban_Box *entrance);

    public:
    TunnelMacroCreator(Sokoban_Board *_the_board);
    ~TunnelMacroCreator() {}
    void compute_macros();
};
