#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "types.h"

// Simple fixed-size hash table used by evaluation modules
template<typename EntryT, std::size_t Size>
class HashTable {
public:
    HashTable() : table(Size) {}

    EntryT* operator[](Key key) {
        std::size_t idx = (std::size_t)key & (Size - 1);
        return &table[idx];
    }

private:
    static_assert((Size & (Size - 1)) == 0, "Size must be power of two");
    std::vector<EntryT> table;
};

// Phase material limits used by material.cpp
constexpr Value MidgameLimit = 15581;
constexpr Value EndgameLimit = 3998;

