#pragma once
//Transmutationtable designed to (hopefully) massively speed up search by saving and comparing states in a hash table
//Try to make to size of the hashtable fit in cache, as this will probably be pretty important for the access time.
//We use uint128_t for full key, which allows for max 9 boxes. Disables trans table if this is not enough.
//(or change to larger integer type, boost support up to 1024.)

#include "Sokoban_Board.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <list>
#include <limits>
#define MAX_BOXES 20
#define BITS_PER_COORD 6

#define BITS_TO_KEY ((MAX_BOXES + 1) * BITS_PER_COORD * 2)

namespace b_mp = boost::multiprecision;


#if BITS_TO_KEY <= 128
typedef b_mp::uint128_t key_type;
#elif BITS_TO_KEY <= 256
typedef b_mp::uint256_t key_type;
#elif BITS_TO_KEY <= 512
typedef b_mp::uint512_t key_type;
#elif BITS_TO_KEY <= 1024
typedef b_mp::uint1024_t key_type;
#else
#error No available type to make the key.
#endif

#define CLOSED 0
#define OPEN 1

struct state_entry;
struct state_entry
{
    key_type full_key; //The full board state. The state contains the position of every box (12 bit per box, limiting the size of the map to 64*64).
                            //The last position is the uppermose, leftmost, position reachable by the player. This means,
                            //That we will be able to hash states which is not excatly equal, as the player position may be different, and match it to the same state.
    float cost_to_state;
    float heuristic;
    uint8_t state = OPEN;
    //These fields are used for tree traversal to get from one state to another.
    //How to get from one state to another:
    //I = initial node, the node we want to move away from.
    //G = goal node, the node we want to get too.
    //For the deepest node, travel following the parent pointers backwards untill we are at the same depth as the other node
    //Always safe the needed steps to the goal node.
    //Now, each node is at equal depth.
    //Travel backwards from each node, untill we find a common ancestor (there should always be one at some point. It is easy to compare ancestors, just compare the full node key.)
    //Beware, we may have adjusted parent key depth, as a faster path was found, thus for each step, be sure that we travel backwards untill the cost is equal for both nodes.
    //A parent node should not be able to be deeper than a child, so this should be no problem.


    move last_move; //Define the last move performed to get to this state. Used for tree traversal

    state_entry *parent_entry; //Defines the the entry which we will get to by performing last move.
                              //This can be used because we don't delete moves.
                              //

    uint32_t total_moves = 0; //Total number of moves performed to this state. Also used for tree traversal

    static key_type create_full_key(const Sokoban_Board &board)
    {
        key_type key = 0;
        for(auto &_box : board.board_boxes)
        {
            auto &box = *_box.first;
            key += box.pos.x_pos;
            key <<= BITS_PER_COORD;
            key += box.pos.y_pos;
            key <<= BITS_PER_COORD;
        }
        key += board.player_box->pos.x_pos;
        key <<= BITS_PER_COORD;
        key += board.player_box->pos.y_pos;
        return key;
    }
    //Comparison function for priority queue
    float get_cost_estimate()
    {
        return this->heuristic + this->cost_to_state;
    }
    bool operator() (state_entry* first, state_entry* second)
    {   //return true if first has a lower estimated cost then second
        //The estimated cost is heuristic + cost_to_state
        if( first->get_cost_estimate() == second->get_cost_estimate() )
        {
            return (void *)first > (void *)second;
        }
        return first->get_cost_estimate() < second->get_cost_estimate();
    }
};

struct bucket
{
    std::list<state_entry> entries;
    float insert_entry(key_type _full_key, float _cost_to_state, move &last_move, state_entry *parent_node, uint32_t depth, state_entry* &this_node, const Sokoban_Board &board)
    {
        //static uint32_t insertions = 0;
        //std::cout << insertions++ << "\t" << _full_key % 1000003 <<std::endl;
        state_entry new_entry;
        new_entry.state = OPEN;
        new_entry.full_key = _full_key;
        new_entry.cost_to_state = _cost_to_state;
        auto &non_const_board = const_cast <Sokoban_Board &>(board);
        new_entry.heuristic = non_const_board.get_heuristic();
        new_entry.heuristic = this->fake_heuristic(&new_entry);
        new_entry.last_move = last_move;
        new_entry.parent_entry = parent_node;
        new_entry.total_moves = depth;
        entries.push_back(new_entry);
        this_node = &entries.back();
        return new_entry.heuristic;
    }

    float fake_heuristic(state_entry *entry)
    {
        /*if(entry->full_key == 36882296192643268) return 150.;
        if(entry->full_key == 36882296175866051) return 144.;
        if(entry->full_key == 36886625536454788) return 144.;
        if(entry->full_key == 36886556816978052) return 141.;
        if(entry->full_key == 36882296175866051) return 144.;
        if(entry->full_key == 36882296175866051) return 144.;
        if(entry->full_key == 36882296175866051) return 144.;
        if(entry->full_key == 36882296175866051) return 144.;*/
        return entry->heuristic;
    }
    uint32_t get_size()
    {
        return this->entries.size();
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

    state_entry *get_entry(key_type _full_key); //Get entry which matches the key.
                                                       //Return nullptr on no match
    state_entry *get_random_entry();                    //Return a random entry.
                                                       //Make sure that the table is not empty,
                                                       //or this will run forever.
    state_entry *get_entry(const Sokoban_Board &board);

    //check if this state already exists in the table.
    //If it exists, and the saved state has a lower or equal cost than the given cost, return false, meaning
    //that we should not search from here, as a shorter path to this state exists.
    //If it exists, and the saved state has a higher cost, owerwrite the saved cost with the new,
    //And return true. Replace the value pointed to by heuristic with the saved heuristic.
    //If it does not exist, return true, calculate a new heuristic and put it in the memory given in the pointer.
    bool check_table(const Sokoban_Board &board, float cost_to_state, float *heuristic,
        move &last_move, state_entry *parent_node, uint32_t depth, state_entry* &this_node);
    uint32_t get_size()
    {
        uint32_t total_size = 0;
        for(uint32_t i = 0; i < this->table_size; i++) total_size += table[i].get_size();
        return total_size;
    }
    uint32_t get_max_size()
    {
        uint32_t max = 0;
        for(uint32_t i = 0; i < this->table_size; i++) max = std::max(max, table[i].get_size());
        return max;
    }
};
