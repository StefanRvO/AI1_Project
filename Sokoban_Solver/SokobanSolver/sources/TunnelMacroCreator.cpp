#include "TunnelMacroCreator.hpp"
#include <vector>

TunnelMacroCreator::TunnelMacroCreator(Sokoban_Board *_the_board)
{
    this->the_board = _the_board;
}

void TunnelMacroCreator::compute_macros()
{
    //Temporarily remove player and boxes
    for(auto &box : this->the_board->board_boxes)
        box.first->remove_box();
    this->the_board->player_box->remove_player();

    //Actually compute the macros.
    this->compute_macros_internal();
    //Restore boxes and player

    this->the_board->player_box->insert_player();
    for(auto &box : this->the_board->board_boxes)
    {
        box.first->insert_box();
    }
}

void TunnelMacroCreator::compute_macros_internal()
{
    mark_candidates();
    std::vector<Sokoban_Box *> entrances = find_entrances();
    for(auto &box : entrances)
    {
        compute_tunnel_members(box);
        if(box->tunnel_type == Two_Way && box->tunnel_members.size() == 0)
        {
            box->tunnel_type = None;
            continue;
        }
        if(box->tunnel_type == Two_Way)
        {
            create_two_way_macro(box);
        }
        else if(box->tunnel_type == One_Way)
            create_one_way_macro(box);

        //Print the created macro move
        /*std::cout << *box << " ";
        for(auto &the_move : box->macro_move[0])
            std::cout << the_move << " ";
        std::cout << "| ";
        for(auto &the_move : box->macro_move[1])
            std::cout << the_move << " ";
        std::cout << "| ";
        for(auto &the_move : box->macro_move[2])
            std::cout << the_move << " ";
        std::cout << "| ";
        for(auto &the_move : box->macro_move[3])
            std::cout << the_move << " ";

        std::cout << std::endl;*/
    }

}

void TunnelMacroCreator::mark_candidates()
{
    for(uint32_t x = 1; x < this->the_board->size_x; x++)
    {
        for(uint32_t y = 1; y < this->the_board->size_y; y++)
        {
            auto &box = this->the_board->board[x][y];
            if(box.is_solid() || box.is_in_deadlock_zone()) continue;
            if(box.is_tunnel())
            {
                if(box.type != Goal)
                {
                    if(box.is_tunnel(Orientation::Horizontal))
                    {
                        box.tunnel_orientation = Orientation::Horizontal;
                    }
                    else box.tunnel_orientation = Orientation::Vertical;
                    box.tunnel_type = set_tunnel_type(&box);
                }
            }
        }
    }
    return;
}

Tunnel_Type TunnelMacroCreator::set_tunnel_type(Sokoban_Box *box)
{
    Sokoban_Box *old_player_box = this->the_board->player_box;
    Sokoban_Box *player_box = nullptr;
    Sokoban_Box *opposite_box = nullptr;
    switch(box->tunnel_orientation)
    {
        case Horizontal:
        player_box = box->nb_left;
        opposite_box = box->nb_right;
        break;
        case Vertical:
        player_box = box->nb_up;
        opposite_box = box->nb_down;
        break;
    }
    player_box->insert_player();
    box->insert_box();
    this->the_board->player_box = player_box;
    this->the_board->calc_reachable(Move_Direction::none);
    Tunnel_Type type;
    if(this->the_board->is_reachable(opposite_box))
    {
        //Two way tunnels need to be real tunnels.
        if(box->is_tunnel(false))
            type = Tunnel_Type::Two_Way;
        else
            type = None;
    }
    else
        type = Tunnel_Type::One_Way;


    this->the_board->player_box = old_player_box;
    box->remove_box();
    player_box->remove_player();
    return type;
}

std::vector<Sokoban_Box *> TunnelMacroCreator::find_entrances()
{
    std::vector<Sokoban_Box *> entrances;
    for(uint32_t x = 1; x < this->the_board->size_x; x++)
    {
        for(uint32_t y = 1; y < this->the_board->size_y; y++)
        {
            auto &box = this->the_board->board[x][y];
            if(box.tunnel_type == None) continue;
            if(box.tunnel_orientation == Horizontal)
            {
                if(box.nb_left->tunnel_type != box.tunnel_type ||
                   box.nb_right->tunnel_type != box.tunnel_type)
                   entrances.push_back(&box);
            }
            else
            {
                if(box.nb_up->tunnel_type != box.tunnel_type ||
                   box.nb_down->tunnel_type != box.tunnel_type)
                   entrances.push_back(&box);
            }
        }
    }
    return entrances;
}

void TunnelMacroCreator::compute_tunnel_members(Sokoban_Box *entrance)
{
    Sokoban_Box *nb;
    Move_Direction tunnel_dir = Move_Direction::none;
    if(entrance->tunnel_orientation == Horizontal)
    {
        if(entrance->nb_left->tunnel_type == entrance->tunnel_type)
            tunnel_dir = left;
        else if(entrance->nb_right->tunnel_type == entrance->tunnel_type)
            tunnel_dir = right;
        else
        {
            return; //Single box tunnel
        }
    }
    else
    {
        if(entrance->nb_up->tunnel_type == entrance->tunnel_type)
            tunnel_dir = up;
        else if(entrance->nb_down->tunnel_type == entrance->tunnel_type)
            tunnel_dir = down;
        else
            return; //Single box tunnel
    }
    //std::cout << tunnel_dir << std::endl;

    nb = entrance->get_neighbour(tunnel_dir);
    while(nb->tunnel_type == entrance->tunnel_type)
    {
        entrance->tunnel_members.push_back(nb);
        nb = nb->get_neighbour(tunnel_dir);
    }
    return;
}

void TunnelMacroCreator::create_two_way_macro(Sokoban_Box* &entrance)
{
    //Find direction the tunnel is in.
    Move_Direction tunnel_dir = none;
    if(entrance->tunnel_members.front() == entrance->nb_up) tunnel_dir = up;
    else if(entrance->tunnel_members.front() == entrance->nb_down) tunnel_dir = down;
    else if(entrance->tunnel_members.front() == entrance->nb_left) tunnel_dir = left;
    else if(entrance->tunnel_members.front() == entrance->nb_right) tunnel_dir = right;

    //the entrance should be the square square, as it may be used for parking.

    Sokoban_Box *real_entry = entrance->get_neighbour(tunnel_dir);

    //Add the entrance itself to the macro.
    real_entry->macro_move[(int)tunnel_dir].push_back(move(tunnel_dir, real_entry));

    //Add the members to the macro
    for(auto &member : entrance->tunnel_members)
    {
        if(member == real_entry) continue;
        real_entry->macro_move[(int)tunnel_dir].push_back(move(tunnel_dir, member));
    }
    entrance = real_entry;
}
void TunnelMacroCreator::create_one_way_macro(Sokoban_Box *entrance)
{
    //Check if this is a single block tunnel
    if(entrance->tunnel_members.size() == 0)
    {
        for(int dir = 0; dir <= (int)Move_Direction::right; dir++)
        {
            entrance->macro_move[dir].push_back(move((Move_Direction)dir, entrance));
        }
        return;
    }
    //Find direction the tunnel is in.
    Move_Direction tunnel_dir = none;
    if(entrance->tunnel_members.front() == entrance->nb_up) tunnel_dir = up;
    else if(entrance->tunnel_members.front() == entrance->nb_down) tunnel_dir = down;
    else if(entrance->tunnel_members.front() == entrance->nb_left) tunnel_dir = left;
    else if(entrance->tunnel_members.front() == entrance->nb_right) tunnel_dir = right;

    //Add the entrance itself to the macro.
    entrance->macro_move[(int)tunnel_dir].push_back(move(tunnel_dir, entrance));
    //Add the members to the macro
    for(auto &member : entrance->tunnel_members)
    {
        entrance->macro_move[(int)tunnel_dir].push_back(move(tunnel_dir, member));
    }
    //Add the box after last member to the macro, but only if the last member is a "true" tunnel square, and the next
    //field is not a goal
    if(entrance->tunnel_members.back()->is_tunnel(false) == true &&
    entrance->tunnel_members.back()->get_neighbour(tunnel_dir)->type != Goal)
    {
        entrance->macro_move[(int)tunnel_dir].push_back(move(tunnel_dir,
            entrance->tunnel_members.back()->get_neighbour(tunnel_dir)));
    }
}
