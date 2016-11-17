#include "TransmutationTable.hpp"
#include <cstdlib>
//#define ID //Use itterative deepening

TransmutationTable::TransmutationTable(uint32_t size)
{
    this->table_size = size;
    this->table = new bucket[this->table_size];
    this->clear();
}

TransmutationTable::~TransmutationTable()
{
    if(this->table) delete[] this->table;
}
void TransmutationTable::clear()
{
    //Set all entries to invalid
    for(uint32_t i = 0; i < this->table_size; i++)
    {
        bucket &tmp = this->table[i];
        tmp.entries.clear();
    }
}


bool TransmutationTable::check_table(const Sokoban_Board &board, float cost_to_state, float *heuristic,
    move &last_move, state_entry *parent_node, uint32_t depth, state_entry* &this_node)
{
    //static uint32_t table_checks = 0;
    //if(table_checks++ % 10000 == 0)std::cout << table_checks  <<std::endl;

    //Calculate the full key
    key_type full_key = state_entry::create_full_key(board);
    //std::cout << full_key << std::endl;
    //Check if the entry exists
    bucket *the_bucket = this->table +(size_t)(full_key % this->table_size);
    for(state_entry &entry : the_bucket->entries)
    {
        if(entry.full_key == full_key)
        {
            #ifdef ID
            if(entry.state == OPEN && entry.cost_to_state >= cost_to_state)
            #else
            if(entry.state == OPEN && entry.cost_to_state > cost_to_state)
            #endif
            {
                //Remember to update cost to state outside
                //entry.cost_to_state = cost_to_state;
                entry.last_move = last_move;
                entry.parent_entry = parent_node;
                entry.total_moves = depth;
                *heuristic = entry.heuristic;
                this_node = &entry;
                //std::cout << this_node << std::endl;

                return true;
            }
            else
            {
                this_node = &entry;
                //std::cout << this_node << std::endl;

                return false;
            }
        }
    }
    //Insert into bucket, overwrite heuristic and return true
    *heuristic = the_bucket->insert_entry(full_key, cost_to_state, last_move, parent_node, depth, this_node, board);
    //std::cout << this_node << std::endl;
    return true;
}

state_entry *TransmutationTable::get_entry(key_type _full_key)
{
    bucket *the_bucket = this->table +(size_t)(_full_key % this->table_size);
    for(state_entry &entry : the_bucket->entries)
    {
        if(entry.full_key == _full_key)
        {
            return &entry;
        }
    }
    return nullptr;
}

state_entry *TransmutationTable::get_random_entry()
{
    while(true)
    {
        bucket *the_bucket = this->table +(size_t)(std::rand() % this->table_size);
        for(state_entry &entry : the_bucket->entries)
        {
            return &entry;
        }
    }
    return nullptr;
}

state_entry *TransmutationTable::get_entry(const Sokoban_Board &board)
{
    key_type full_key = state_entry::create_full_key(board);
    return this->get_entry(full_key);
}
