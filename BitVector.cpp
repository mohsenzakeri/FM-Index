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

    for (int i = 0; i < 1000; i++)
        b.set_index(i);
    b.reset_index(5);
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
    std::cerr<<"r(5)  r(0)  r(14) r(20) \n";
    std::cerr<<r(5)<< "\n"<< r(0) << "\n"<< r(14) << "\n"<< r(20) <<"\n";
    std::cerr<<r.rank1(12) << " "<< r.rank0(12) << "\n";

    std::cerr<<r.overhead() << "overhead\n" << b.get_arr().size()*64 << "\n";
    //b.show_range(0,10);
    std::string fname = "mohsen.bin";
    r.save(fname);

    rank_support r1;
    r1.load(fname);
    //r1.get_bit_vec()->show();
    std::cerr<<r1.rank1(222) << " " << r1.rank0(12) << " " << r1.rank1(19) << "\n";
    std::cerr<<r.rank1(222) << " "  << r.rank0(12) << " " << r.rank1(19) << "\n";

    select_support s1(&r1);
    std::cerr<<s1(1) << " " << s1(10) <<  " " << s1(22) << " " << s1(100) << "\n";
    r.get_bit_vec()->reset_index(35);
    //r.get_bit_vec()->show();
    std::cerr<<"here: s1[999]:" << s1(999) << " s1[5]:" << s1(5) << " r1[63]:" << r1(63) << " " << r(63) << " bit[35]:" << r1.get_bit_vec()->read_bit(35) <<  " bit[35]:" << b.read_bit(35) << "\n";
    //r1.get_bit_vec()->reset_index(231);
    std::cerr<<"s1[205]:"<<s1(205)<<" s1[1205]:" << s1(1000)<<"\n";
}
