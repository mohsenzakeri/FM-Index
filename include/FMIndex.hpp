#ifndef __FMINDEX__
#define __FMINDEX__

#include <divsufsort.h>

#include "BitVector.hpp";
#include "RankSupport.hpp";
#include "SelectSupport.hpp";

class FMIndex {
public:
private:
    unsigned char* BWT;
    unsigned char* alphabet;
    uint64_t counts;
    uint64_t length;
    std::vector<bit_vector*> occs;
};

#endif
