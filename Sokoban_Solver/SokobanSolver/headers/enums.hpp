#pragma once
#include <cstdint>


enum Tunnel_Type : uint8_t
{
    None,
    One_Way,
    Two_Way,
};

enum Orientation : uint8_t
{
    Horizontal,
    Vertical,
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
    none,
};

enum Move_Type
{
    Normal,
    Macro,
    Invalid,
};
