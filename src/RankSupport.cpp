#include "RankSupport.hpp"

#include <bitset>

rank_support::rank_support() {
    num_bits =0;
    superblock_size =0;
    block_size =0;
    superblock_count =0;
    block_count =0;
    block_count_per_superblock = 0;
}
rank_support::~rank_support() {
}

rank_support::rank_support(bit_vector* b) : bit_vec(b) {
    num_bits = bit_vec->get_length();
    superblock_size = std::floor((std::pow(std::log2(num_bits),2))/2) + 1;

    block_size = std::floor((std::log2(num_bits))/2) + 1;

    superblock_count = static_cast<uint64_t>(num_bits/superblock_size) + 1;

    auto div_ = std::div(static_cast<int>(superblock_size), static_cast<int>(block_size));
    block_count_per_superblock = div_.rem ? div_.quot + 1 : div_.quot; 
    block_count = block_count_per_superblock * superblock_count;

    uint64_t superblock_bit_size = static_cast<uint64_t>(std::ceil(std::log2(num_bits)));
    uint64_t block_bit_size = static_cast<uint64_t>(std::ceil(std::log2(superblock_size)));
    Rs = bit_vector(superblock_count, superblock_bit_size); 
    Rb = bit_vector(block_count, block_bit_size);
    uint64_t superblock_index = 0;
    uint64_t block_index = 0;
    uint64_t current_superblock = 0;
    uint64_t current_block = 0;
    uint64_t superblock_iter = 0;

    for (uint64_t i = 0; i < num_bits; i++) {
        if (i % superblock_size == 0) {
	    // Flush current superblock in Rs
	    Rs.set_value(superblock_index, current_superblock);
	    // Increase the superblock index
	    superblock_index += 1;
	}
	// Store the next bit in the current_superblock
	current_superblock += b->read_bit(i);
    }

    for (uint64_t i = 0; i < num_bits; i+=superblock_size) {
        for (uint64_t j = 0; j < superblock_size; j++) {
	    if (block_index > block_count or j + i >= num_bits) break;
	    if (j % block_size == 0) {// or j == superblock_size - 1) {
		// Store current blick in Rb
		Rb.set_value(block_index, current_block);
		// increase block index
		block_index += 1;
	    }
	    // Store the next bit in current_block
	    current_block += b->read_bit(j + i);
	}
	current_block = 0;
    }
    /*uint32_t Rps_size = std::pow(2, block_size);
    for (uint8_t i = 0; i < Rps_size; i++) {
	std::vector<uint8_t> Rp;
	Rps.push_back(Rp);
    }
    for (uint32_t i = 0; i < std::pow(2, block_size); i++) {
	uint32_t bit = 1;
	uint8_t curr_rank = 0;
	for (int32_t j = block_size-1; j >= 0; j--) {
	//for (int32_t j = 0; j < block_size; j++) {
            uint32_t checker = bit<<j;
	    if (checker & i)
	        curr_rank += 1;
	    Rps[i].push_back(curr_rank);
	}
    }*/
}

uint64_t rank_support::operator()(uint64_t index) {
    if (index >= num_bits) {
        std::cerr<<"Index out of bound(" << num_bits << ")\n";
        return 0;
    }
    uint64_t superblock_index = index/superblock_size;
    uint64_t in_block_index = std::ceil((index - superblock_index * superblock_size)/block_size);
    uint64_t block_index = in_block_index + superblock_index * block_count_per_superblock;
    uint64_t Rp = 0;
    for (uint64_t i = superblock_size * superblock_index + in_block_index * block_size; i <= index; i++) {
        Rp += bit_vec->read_bit(i);
    }
    //std::cerr<< Rs_[superblock_index] << "----" << static_cast<int>(Rb_[block_index]) << "---" << Rp << "\n";
    return Rs.get_value(superblock_index) + Rb.get_value(block_index) + Rp;
}

uint64_t rank_support::rank1(uint64_t index) {
    return (*this)(index);
}

uint64_t rank_support::rank0(uint64_t index) {
    return index + 1 - this->rank1(index);
}

uint64_t rank_support::overhead() {
    uint64_t rps_size = 0;
    return Rs.get_size_in_bits() + Rb.get_size_in_bits() + rps_size + 6*64;
}

bool rank_support::save(std::string& fname) {
    std::ofstream fout(fname, std::ios::out | std::ios::binary);

    uint64_t length = bit_vec->get_length();
    fout.write((char*) &length, sizeof(length));
    auto arr = bit_vec->get_arr();
    for (auto& item : arr)
        fout.write((char*) &item, sizeof(item));
    fout.write((char*) &superblock_size, sizeof(superblock_size));
    fout.write((char*) &block_size, sizeof(block_size));
    fout.write((char*) &superblock_count, sizeof(superblock_count));
    fout.write((char*) &block_count, sizeof(block_count));
    fout.write((char*) &block_count_per_superblock, sizeof(block_count_per_superblock));
    uint64_t arr_size = Rs.get_arr().size();
    fout.write((char*) &arr_size, sizeof(arr_size));
    length = Rs.get_length();
    fout.write((char*) &length, sizeof(length));
    uint8_t bsize = Rs.get_bsize();
    fout.write((char*) &bsize, sizeof(bsize));
    for (auto& item : Rs.get_arr()) {
        fout.write((char*) &item, sizeof(item));
    }
    arr_size = Rb.get_arr().size();
    fout.write((char*) &arr_size, sizeof(arr_size));
    length = Rb.get_length();
    fout.write((char*) &length, sizeof(length));
    bsize = Rb.get_bsize();
    fout.write((char*) &bsize, sizeof(bsize));
    for (auto& item : Rb.get_arr()) {
        fout.write((char*) &item, sizeof(item));
    }
    fout.close();
    return true;
}

bool rank_support::load(std::string& fname) {
    std::ifstream fin(fname, std::ios::in | std::ios::binary);

    fin.read((char*) &num_bits, sizeof(num_bits));
    bit_vec = new bit_vector(num_bits);
    uint64_t arr_length = static_cast<uint64_t>(num_bits/64) + 1;
    for (uint64_t i = 0; i < arr_length; i++) {
        uint64_t arr_item = 0;
	fin.read((char*) &arr_item, sizeof(arr_item));
	bit_vec->set_arr(i, arr_item);
    }
    fin.read((char*) &superblock_size, sizeof(superblock_size));
    fin.read((char*) &block_size, sizeof(block_size));
    fin.read((char*) &superblock_count, sizeof(superblock_count));
    fin.read((char*) &block_count, sizeof(block_count));
    fin.read((char*) &block_count_per_superblock, sizeof(&block_count_per_superblock));
    uint64_t arr_size = 0;
    fin.read((char*) &arr_size, sizeof(arr_size));
    uint64_t length = 0;
    fin.read((char*) &length, sizeof(length));
    uint8_t bsize = 0;
    fin.read((char*) &bsize, sizeof(bsize));
    Rs = bit_vector(length, static_cast<uint64_t>(bsize)); 
    for (uint64_t i = 0; i < arr_size; i++) {
        uint64_t superblock_item = 0;
	fin.read((char*)& superblock_item, sizeof(superblock_item));
	Rs.set_arr(i, superblock_item);
    }
    fin.read((char*) &arr_size, sizeof(arr_size));
    fin.read((char*) &length, sizeof(length));
    fin.read((char*) &bsize, sizeof(bsize));
    Rb = bit_vector(length, static_cast<uint64_t>(bsize)); 
    for (uint64_t i = 0; i < arr_size; i++) {
        uint64_t block_item = 0;
	fin.read((char*)& block_item, sizeof(block_item));
	Rb.set_arr(i, block_item);
    }
    fin.close();
    return true;
}
