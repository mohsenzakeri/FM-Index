#ifndef __SELECT_SUPPORT__
#define __SELECT_SUPPORT__

#include "RankSupport.hpp"

class select_support {
public:
    select_support(rank_support* r) : rs(r) {}
    uint64_t operator()(uint64_t index);
    uint64_t select1(uint64_t index);
    uint64_t select0(uint64_t index);
    uint64_t overhead();
    bool save(std::string& fname);
    bool load(std::string& fname);
private:
    rank_support* rs;
};

#endif
