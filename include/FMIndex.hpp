#ifndef __FMINDEX__
#define __FMINDEX__

#include <map>

#include <divsufsort.h>

#include "BitVector.hpp"
#include "RankSupport.hpp"
#include "SelectSupport.hpp"

struct move_row{
    move_row(uint32_t p, uint32_t n, 
                 uint32_t pp, uint32_t id, char c) {
        this->p = p;
        this->n = n;
        this->pp = pp;
        this->id = id;
        this->c = c;
    }
    uint32_t p;
    uint32_t n;
    uint32_t pp;
    uint32_t id;
    char c;
};

class FMIndex {
public:
    FMIndex() { }
    FMIndex(char* input_file, bool move);
    void build(char* input_text);
    void save(char* output_dir);
    void load(char* index_dir);
    bool query(std::string pattern, uint64_t& from_, uint64_t& to);
    uint64_t* check_next(uint64_t from, uint64_t to, char c);

    void build_move(char* input_text);
    uint32_t LF(uint32_t row);
    int32_t move_rl_query_ms(std::string R, std::vector<uint32_t>& ms_lens);
    int32_t get_index(uint32_t i);
    int32_t fast_forward(uint32_t pointer, uint32_t index);
private:
    unsigned char* bwt;
    std::string S;
    std::vector<unsigned char> alphabet;
    std::vector<uint64_t> counts;
    uint64_t length;
    std::vector<bit_vector*> occs;
    std::vector<rank_support*> occs_rank;
    std::map<unsigned char, uint32_t> alphamap;

    int *SA;
    std::vector<move_row> rlbwt;
};

#endif
