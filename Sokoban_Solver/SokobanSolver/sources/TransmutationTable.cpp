#include "TransmutationTable.hpp"

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
        for(uint8_t j = 0; j < BUCKET_SIZE; j++)
        {
            tmp.valid[j] = false;
        }
    }
}


bool TransmutationTable::check_table(const Sokoban_Board &board, const Position &upper_left, uint32_t cost_to_state, int32_t *heuristic)
{
    //static uint32_t table_checks = 0;
    //if(table_checks++ % 10000 == 0)std::cout << table_checks  <<std::endl;

    //Calculate the full key
    b_mp::uint128_t full_key = state_entry::create_full_key(board, upper_left);
    //std::cout << full_key << std::endl;
    //Check if the entry exists
    bucket *the_bucket = this->table +(size_t)(full_key % this->table_size);
    for(uint8_t i = 0; i < BUCKET_SIZE; i++)
    {
        state_entry &entry = the_bucket->entries[i];
        if(the_bucket->valid[i] && entry.full_key == full_key)
        {
            if(entry.cost_to_state >= cost_to_state)
            {
                entry.cost_to_state = cost_to_state;
                *heuristic = entry.heuristic;
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    //Insert into bucket, overwrite heuristic and return true
    *heuristic = the_bucket->insert_entry(full_key, cost_to_state, board);
    return true;
}
