#ifndef __FMINDEX__
#define __FMINDEX__

#include <map>

#include <divsufsort.h>

#include "BitVector.hpp"
#include "RankSupport.hpp"
#include "SelectSupport.hpp"

class FMIndex {
public:
    FMIndex() { }
    FMIndex(char* input_file);
    void build(char* input_text);
    void save(char* output_dir);
    void load(char* index_dir);
    bool query(char* pattern, uint64_t& from_, uint64_t& to);
    uint64_t* check_next(uint64_t from, uint64_t to, char c);
private:
    unsigned char* bwt;
    std::vector<unsigned char> alphabet;
    std::vector<uint64_t> counts;
    uint64_t length;
    std::vector<bit_vector*> occs;
    std::vector<rank_support*> occs_rank;
    std::map<unsigned char, uint32_t> alphamap;
};

#endif
