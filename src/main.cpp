#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <divsufsort.h>

#include "BitVector.hpp"
#include "RankSupport.hpp"
#include "SelectSupport.hpp"

int main() {
    std::cerr<<__cplusplus<<"\n";
    bit_vector bbb(1000,65);
    bit_vector bb(1000,5);
    bb.set_value(2,12);
    bb.set_value(3,15);
    bb.set_value(2,2);
    std::cerr << "[2]: "<< bb.get_value(2)<<"\n";
    std::cerr << "[3]: "<< bb.get_value(3)<<"\n";
    std::cerr << "[0]: "<< bb.get_value(0)<<"\n";
    std::cerr << "[1]: "<< bb.get_value(1)<<"\n";
    bb.set_value(9,31);
    std::cerr<<bb.get_value(9)<<" didit work?\n";
    for (int i = 0; i < 1000; i++) {
        bb.set_value(i, (i*7)%32);
    }
    for (int i = 0; i < 1000; i++) {
        bb.set_value(i, (i*5)%32);
    }
    for (int i = 0; i < 1000; i++) {
        if (bb.get_value(i) != (i*5)%32)
	    std::cerr<<"error!!!!\n";
    }
    std::cerr<<"SUCCESS!\n";

    bit_vector b(2000000);
    //b.show();

    for (int i = 0; i < 1000; i+=3)
        b.set_index(i);
    //b.reset_index(5);
    b.show_range(0,20);
    /*b.set_index(0);
    b.set_index(10);
    b.set_index(11);
    b.set_index(3);
    b.set_index(1);
    b.set_index(2);*/
    //b.show();
    //b.reset_index(6);
    //b.reset_index(6);
    //b.show();

    rank_support r(&b);
    std::cerr<<r(1000);
    std::cerr<<"r(5)  r(0)  r(14) r(1200) \n";
    std::cerr<<r(5)<< "|"<< r(0) << "|"<< r(14) << "|"<< r(1200) <<"\n";
    std::cerr<<r.rank1(12) << " "<< r.rank0(12) << "\n";

    std::cerr<<r.overhead() << "overhead\n" << b.get_arr().size()*64 << "\n";
}
