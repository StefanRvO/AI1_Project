#pragma once

struct TableEntry
{
    int32_t heuristic;
    uint64_t hash_key;
}


class TransmutationTable
{
    private:
        uint32_t size = 0;
        TableEntry
    public:
        TransmutationTable(uint32_t _size);
}
