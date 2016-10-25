#pragma once
//Transmutationtable designed to (hopefully) massively speed up search by saving and comparing states in a hash table
//Try to make to size of the hashtable fit in cache, as this will probably be pretty important for the access time.
//We use uint128_t for full key, which allows for max 9 boxes. Disables trans table if this is not enough.
//(or change to larger integer type, boost support up to 1024.)

#include "Sokoban_Board.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#define BUCKET_SIZE 4
#define REPLACE
#define BITS_PER_COORD 6

namespace b_mp = boost::multiprecision;

struct state_entry;
struct state_entry
{
    b_mp::uint128_t full_key; //The full board state. The state contains the position of every box (12 bit per box, limiting the size of the map to 64*64).
                            //The last position is the uppermose, leftmost, position reachable by the player. This means,
                            //That we will be able to hash states which is not excatly equal, as the player position may be different, and match it to the same state.
    uint32_t cost_to_state;
    int32_t heuristic;
    static b_mp::uint128_t create_full_key(const Sokoban_Board &board, const Position &upper_left)
    {
        b_mp::uint128_t key = 0;
        for(auto &_box : board.board_boxes)
        {
            auto &box = *_box.first;
            key += box.pos.x_pos;
            key <<= BITS_PER_COORD;
            key += box.pos.y_pos;
            key <<= BITS_PER_COORD;
        }
        key += upper_left.x_pos;
        key <<= BITS_PER_COORD;
        key += upper_left.y_pos;
        return key;
    }
    int32_t replace_entry(b_mp::uint128_t _full_key, uint32_t _cost_to_state, const Sokoban_Board &board)
    {
        //Remove constness for the board, as we can't make get_heuristic const.
        //This is OK!
        auto &non_const_board = const_cast <Sokoban_Board &>(board);
        this->heuristic = non_const_board.get_heuristic();
        this->cost_to_state = _cost_to_state;
        this->full_key = _full_key;
        return this->heuristic;
    }
};

struct bucket
{
    state_entry entries[BUCKET_SIZE];
    bool valid[BUCKET_SIZE];
    uint8_t counter = 0;

    int32_t insert_entry(b_mp::uint128_t _full_key, uint32_t _cost_to_state, const Sokoban_Board &board)
    {
        //static uint32_t insertions = 0;
        //std::cout << insertions++ << "\t" << _full_key % 1000003 <<std::endl;
        if(this->valid[this->counter] == false)
        {
            return this->replace_entry(_full_key, _cost_to_state, board);
        }
        else
        {
            #ifdef REPLACE
            return this->replace_entry(_full_key, _cost_to_state, board);
            #else
            //Remove constness for the board, as we can't make get_heuristic const.
            //This is OK!
            auto &non_const_board = const_cast <Sokoban_Board &>(board);
            int32_t heuristic = non_const_board.get_heuristic();
            return heuristic;
            #endif
        }
    }

    int32_t replace_entry(b_mp::uint128_t _full_key, uint32_t _cost_to_state, const Sokoban_Board &board)
    {
        //static uint32_t collisions = 0;
        //if(this->valid[this->counter])std::cout << collisions++ << "\t" << _full_key % 1000003 <<std::endl;
        int32_t heuristic = entries[this->counter].replace_entry(_full_key, _cost_to_state, board);
        this->valid[this->counter++] = true;
        if(this->counter == BUCKET_SIZE) this->counter = 0;
        return heuristic;
    }
};


class TransmutationTable
{
    public:
    uint32_t table_size;
    bucket *table = nullptr;
    TransmutationTable(uint32_t size);
    ~TransmutationTable();
    void clear();
    //check if this state already exists in the table.
    //If it exists, and the saved state has a lower cost than the given cost, return false, meaning
    //that we should not search from here, as a shorter path to this state exists.
    //If it exists, and the saved state has a higher cost, owerwrite the saved cost with the new,
    //And return true. Replace the value pointed to by heuristic with the saved heuristic.
    //If it does not exist, return true, calculate a new heuristic and put it in the memory given in the pointer.
    bool check_table(const Sokoban_Board &board, const Position &upper_left, uint32_t cost_to_state, int32_t *heuristic);
};
