#ifndef __FMINDEX__
#define __FMINDEX__

#include <divsufsort.h>

#include "BitVector.hpp";
#include "RankSupport.hpp";
#include "SelectSupport.hpp";

class FMIndex {
public:
    FMIndex(char* input_file);
    void build(char* input_text);
    void save(char* output_dir);
private:
    unsigned char* bwt;
    std::vector<unsigned char> alphabet;
    std::vector<uint64_t> counts;
    uint64_t length;
    std::vector<bit_vector*> occs;
};

#endif
