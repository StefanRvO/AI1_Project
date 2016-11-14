#include "TunnelMacroCreator.hpp"
#include <vector>

TunnelMacroCreator::TunnelMacroCreator(Sokoban_Board *_the_board)
{
    this->the_board = _the_board;
}

void TunnelMacroCreator::compute_macros()
{
    //Temporarily remove player and boxes
    std::vector<Box_Type> types;
    for(auto &box : this->the_board->board_boxes)
    {
        types.push_back(box.first->type);
        box.first->type = Box_Type::Free;
    }
    types.push_back(this->the_board->player_box->type);
    switch(this->the_board->player_box->type)
    {
        case Player_On_Goal:
        case Player:
            this->the_board->player_box->type = Box_Type::Free;
            break;
        case DeadLock_Zone_Player:
            this->the_board->player_box->type = Box_Type::DeadLock_Zone_Free;
            break;
        default:
            assert(false);
    }
    //Actually compute the macros.
    this->compute_macros_internal();
    //Restore boxes and player
    this->the_board->player_box->type = types.back();
    types.pop_back();
    uint32_t i = 0;
    for(auto &box : this->the_board->board_boxes)
    {
        box.first->type = types[i++];
    }
}

void TunnelMacroCreator::compute_macros_internal()
{
    mark_candidates();
    std::vector<Sokoban_Box *> entrances = find_entrances();
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
        type = Tunnel_Type::Two_Way;
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

    return entrances;
}
