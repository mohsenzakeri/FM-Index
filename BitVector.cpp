#include <iostream>
#include <bitset>

#include "BitVector.hpp"
#include "RankSupport.hpp"
#include "SelectSupport.hpp"

bit_vector::bit_vector(uint64_t l) : length(l), bsize(1) {
    uint64_t arr_length = static_cast<uint64_t>(length/64) + 1;
    arr = std::vector<uint64_t>(arr_length, 0);
}

bit_vector::bit_vector(uint64_t l, uint64_t bs) {
    if (bs > 64) {
        std::cerr<<"The block length greater than 64 is not supported!\n";
	return;
    }
    length = l;
    bsize = bs;
    uint64_t arr_length = static_cast<uint64_t>(length*bsize/64) + 1;
    arr = std::vector<uint64_t>(arr_length, 0);
}

uint64_t get_range(uint64_t arr_item, uint64_t arr_index_from, uint64_t arr_index_to) {
    uint64_t finder = 0;
    for(auto i = arr_index_from; i <= arr_index_to; i++) {
        uint64_t bit = 1;
	bit <<= i;
	finder |= bit;
    }
    return static_cast<uint64_t>((finder&arr_item)>>arr_index_from);
}

uint64_t bit_vector::get_value(uint64_t index) {
    uint64_t from = index*bsize;
    uint64_t to = (index+1)*bsize-1;
    uint64_t arr_number_from = static_cast<uint64_t>(from/64);
    uint64_t arr_number_to = static_cast<uint64_t>(to/64);
    if (arr_number_from == arr_number_to) {
        uint64_t arr_index_from = from%64;
	uint64_t arr_index_to = to%64;
	return get_range(arr[arr_number_from], arr_index_from, arr_index_to);
    } else {
        uint64_t arr_index_from = from%64;
	uint64_t arr_index_to = to%64;   
	uint64_t left_value = get_range(arr[arr_number_to], 0, arr_index_to);
	uint64_t right_value = get_range(arr[arr_number_from], arr_index_from, 63);
	uint64_t finder = 0;

        uint64_t res = static_cast<uint64_t>(left_value<<(64-arr_index_from));
	res += right_value;
	return res;
    }
}

uint64_t set_range(uint64_t arr_item, uint64_t arr_index_from, uint64_t arr_index_to, uint64_t value) {
    uint64_t setter = 0; 
    for (auto i = arr_index_from; i <= arr_index_to; i++) {
        uint64_t bit = 1;
        bit <<= i;
        setter |= bit;
    }
    //std::cerr<< arr_index_from << " " << arr_index_to << " \n";
    //std::cerr<<std::bitset<sizeof(uint64_t)*8>(setter)<<"\n";
    setter = ~setter;
    //std::cerr<<std::bitset<sizeof(uint64_t)*8>(setter)<<"\n";
    arr_item &= setter;
    uint64_t new_val = value<<arr_index_from;
    //std::cerr<<std::bitset<64>(value) << "\n" << std::bitset<64>(new_val) << "\n";
    arr_item |= new_val;
    return arr_item;
}

void bit_vector::set_value(uint64_t index, uint64_t value) {
    if (value >= 1<<bsize) {
        std::cerr<<"Value is greater than the block size!\n";
        return;
    }

    uint64_t from = index*bsize;
    uint64_t to = (index+1)*bsize-1;
    uint64_t arr_number_from = static_cast<uint64_t>(from/64);
    uint64_t arr_number_to = static_cast<uint64_t>(to/64);
    if (arr_number_from == arr_number_to) {
        uint64_t arr_index_from = from%64;
	uint64_t arr_index_to = to%64;
	arr[arr_number_from] = set_range(arr[arr_number_from], arr_index_from, arr_index_to, value);
    } else {
        uint64_t arr_index_from = from%64;
	uint64_t arr_index_to = to%64;   
	uint64_t left_bits_count = 64 - arr_index_from;
	uint64_t right_bits_count = arr_index_to + 1;
	uint64_t left_value = value >> left_bits_count;
	uint64_t bit = 1;
	uint64_t setter = ((bit<<left_bits_count)-1);
	//std::cerr<<"setter: " << std::bitset<64>(setter) <<"\n";
	uint64_t right_value = setter&value;
	//std::cerr<<"?\n" << std::bitset<64>(value) << "\n" << left_bits_count << " " << std::bitset<64>(left_value) << "\n" << right_bits_count << " " <<  std::bitset<64>(right_value) << "?\n";
	//std::cerr<<std::bitset<64>(arr[arr_number_from]) << "\n" << std::bitset<64>(arr[arr_number_to]) << "\n\n";
	arr[arr_number_from] = set_range(arr[arr_number_from], arr_index_from, 63, right_value);
	arr[arr_number_to] = set_range(arr[arr_number_to], 0, arr_index_to, left_value);
	//std::cerr<<std::bitset<64>(arr[arr_number_from]) << "\n" << std::bitset<64>(arr[arr_number_to]) << "\n\n";
    }
}

void bit_vector::set_index(uint64_t index) {
    if (index >= length) {
        std::cerr<<"Index greater than the length\n";
        return;
    }

    uint64_t arr_number = static_cast<uint64_t>(index/64);
    uint64_t arr_index = index%64;

    // check boundaries
    if (arr_number > arr.size() or arr_index > 63) {
        std::cerr<<"Some kind of error!\n";
	return;
    }
    uint64_t bit = 1;
    uint64_t setter = bit<<arr_index;
    arr[arr_number] = arr[arr_number]|setter;
}

void bit_vector::reset_index(uint64_t index) {
    if (index >= length) {
        std::cerr<<"Index greater than the length\n";
        return;
    }

    uint64_t arr_number = static_cast<uint64_t>(index/64);
    uint64_t arr_index = index%64;

    // check boundaries
    if (arr_number > arr.size() or arr_index > 63) {
        std::cerr<<"Some kind of error!\n";
	return;
    }

    uint64_t resetter = ~(1<<arr_index);
    arr[arr_number] = arr[arr_number]&resetter;
}

void bit_vector::show() {
    for (auto& number : arr) {
        std::cout<< std::bitset<sizeof(uint64_t)*8>(number) << "\n";
    }
}

void bit_vector::show_range(uint64_t from, uint64_t to) {
    if (to > arr.size()) {
        std::cerr<<"Out of range, " << to << " greater than " << arr.size() << "\n";
        to = arr.size();
    }
    for (uint64_t i = from; i < to; i++)
        std::cout<< std::bitset<sizeof(uint64_t)*8>(arr[i]) << "\n";
}

bool bit_vector::read_bit(uint64_t index) {
     if (index >= length) {
        std::cerr<<"Index greater than the length\n";
        return false;
    }

    uint64_t arr_number = static_cast<uint64_t>(index/64);
    uint64_t arr_index = index%64;

    // check boundaries
    if (arr_number > arr.size() or arr_index > 63) {
        std::cerr<<"Some kind of error!\n";
	return false;
    }

    uint64_t bit_finder = static_cast<uint64_t>(1)<<arr_index; 
    return arr[arr_number]&bit_finder;
}
