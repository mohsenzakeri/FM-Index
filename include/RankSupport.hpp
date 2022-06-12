#ifndef __RANK_SUPPORT__
#define __RANK_SUPPORT__

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

#include "BitVector.hpp"

class rank_support {
public:
    rank_support(bit_vector* b);
    rank_support();
    ~rank_support();

    uint64_t operator()(uint64_t index);
    uint64_t rank1(uint64_t index);
    uint64_t rank0(uint64_t index);
    uint64_t overhead();
    bool save(std::string& fname);
    bool load(std::string& fname);
    bit_vector* get_bit_vec() { return bit_vec; }
private:
    bit_vector* bit_vec;
    bit_vector Rs;
    bit_vector Rb;

    uint64_t num_bits;
    uint64_t superblock_size;
    uint64_t block_size;
    uint64_t superblock_count;
    uint64_t block_count;
    uint64_t block_count_per_superblock;
};

#endif
