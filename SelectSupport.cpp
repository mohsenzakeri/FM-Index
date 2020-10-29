#include "SelectSupport.hpp"

uint64_t select_support::operator()(uint64_t index) {
    bit_vector* bit_vec = rs->get_bit_vec();
    uint64_t n = bit_vec->get_length();
    //std::cerr<<rs->rank1(35)<<"rank!";
    if (rs->rank1(n-1) < index) {
        std::cerr<<"No index with this rank value exists!\n";
	return 0;
    }
    uint64_t left = 0;
    uint64_t right = n-1;
    //std::cerr<<"\n";
    uint64_t i = static_cast<uint64_t>((left+right)/2);
    while (true) {
	uint64_t ri = rs->rank1(i);
	//std::cerr<< i << ":" << ri << "\n";
        if (ri == index and bit_vec->read_bit(index)) {
	    right = i;
	} else if (ri >= index) {
	    right = i;
	} else {
	    left = i;
	}
	if (right <= left+1) {
	    return i;
	}
        i = static_cast<uint64_t>((left+right)/2);
    }
}

uint64_t select_support::select1(uint64_t index) {
    return (*this)(index);
}

uint64_t select_support::select0(uint64_t index) {
    bit_vector* bit_vec = rs->get_bit_vec();
    uint64_t n = bit_vec->get_length();
    //std::cerr<<rs->rank1(35)<<"rank!";
    if (rs->rank1(n-1) < index) {
        std::cerr<<"No index with this rank value exists!\n";
	return 0;
    }
    uint64_t left = 0;
    uint64_t right = n-1;
    while (true) {
        uint64_t i = static_cast<uint64_t>((left+right)/2);
	uint64_t ri = rs->rank0(i);
        if (ri == index and bit_vec->read_bit(index)) {
	    return i;
	} else if (ri >= index) {
	    right = i;
	} else {
	    left = i;
	}
	if (right <= left+1) {
	    return 0;
	}
    }
}

uint64_t select_support::overhead() {
    return 0;
}

bool select_support::save(std::string& fname) {
    return rs->save(fname);
}

bool select_support::load(std::string& fname) {
    return rs->load(fname);
}
