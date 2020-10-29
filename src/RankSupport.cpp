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

rank_support::rank_support(bit_vector* b) : bit_vec(b) {
    num_bits = bit_vec->get_length();
    //std::cerr<<num_bits<<" num_bits\n";
    superblock_size = std::floor((std::pow(std::log2(num_bits),2)-1)/2) + 1;
    block_size = std::floor((std::log2(num_bits)-1)/2) + 1;
    //std::cerr<<superblock_size << " " << block_size << " sizes\n";
    superblock_count = static_cast<uint64_t>(num_bits/superblock_size) + 1;
 
    auto div_ = std::div(static_cast<int>(superblock_size), static_cast<int>(block_size));
    block_count_per_superblock = div_.rem ? div_.quot + 1 : div_.quot; 
    //std::cerr<<block_count_per_superblock<<" bcps\n";
    block_count = block_count_per_superblock * superblock_count;

    Rs_ = std::vector<uint64_t>(superblock_count, 0);
    Rb_ = std::vector<uint8_t>(block_count, 0);
    uint64_t superblock_bit_size = static_cast<uint64_t>(std::ceil(std::log2(num_bits)));
    uint64_t block_bit_size = static_cast<uint64_t>(std::ceil(std::log2(superblock_size)));
    //std::cerr<<"superblock_bit_size: " << superblock_bit_size << "block_bit_size: " << block_bit_size << "\n";
    Rs = bit_vector(superblock_count, superblock_bit_size); 
    Rb = bit_vector(block_count, block_bit_size);
    //std::cerr<<superblock_count << " "<< superblock_bit_size << " super\n";
    //std::cerr<<block_count << " "<< block_bit_size << " block\n";
    uint64_t superblock_index = 0;
    uint64_t block_index = 0;
    uint64_t current_superblock = 0;
    uint64_t current_block = 0;
    uint64_t superblock_iter = 0;

    for (uint64_t i = 0; i < num_bits; i++) {
        if (i % superblock_size == 0) {
	    // Flush current superblock in Rs
	    //if (i < 10*superblock_size) std::cerr<<current_superblock<<"\n";
	    Rs_[superblock_index] = current_superblock;
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
		//    if (i < 1010) std::cerr<<block_index << " " << (int)current_block<<" b\n";
		// Store current blick in Rb
	        Rb_[block_index] = current_block;
		Rb.set_value(block_index, current_block);
		// increase block index
		block_index += 1;
	    }
	    // Store the next bit in current_block
	    current_block += b->read_bit(j + i);
	}
	current_block = 0;
    }
    
    /*Rps = std::vector<std::vector<uint8_t> >(std::pow(2, block_size));
    for (uint8_t i = 0; i < std::pow(2, block_size); i++) {
        auto Rp = std::vector<uint8_t>(block_size, 0);
	Rps[i] = Rp;
    }
    for (uint32_t i = 0; i < std::pow(2, block_size); i++) {
        
    }*/

    /*int i = 0;
    for (auto& rs_ : Rs)
        std::cerr<<"rs:" << i++ << " "<< rs_<<"\n";
    i = 0;
    for (auto& rb_ : Rb)
        std::cerr<<"rb:" << i++ << " " << static_cast<uint64_t>(rb_)<<"\n";*/
}

uint64_t rank_support::operator()(uint64_t index) {
    if (index >= num_bits) {
        std::cerr<<"Index out of bound(" << num_bits << ")\n";
        return 0;
    }
    uint64_t superblock_index = index/superblock_size;
    uint64_t in_block_index = std::ceil((index - superblock_index * superblock_size)/block_size);
    uint64_t block_index = in_block_index + superblock_index * block_count_per_superblock;
    //std::cerr<<superblock_index << " " << in_block_index << " " << block_index << "\n";
    uint64_t Rp = 0;
    //std::cerr<<superblock_index << " " << superblock_size << " " << in_block_index << " " << block_size << " " << index << "\n";
    for (uint64_t i = superblock_size * superblock_index + in_block_index * block_size; i <= index; i++) {
        Rp += bit_vec->read_bit(i);
    }
    //std::cerr<<Rs[superblock_count] << " " << (uint64_t)Rb[block_index] << " " << Rp<<"\n";
    //return Rs[superblock_index] + Rb[block_index] + Rp;
    //std::cerr<<Rb.get_value(0) << " "<< Rb.get_value(1) << " " << Rb.get_value(2) << " " << Rb.get_value(3) << "\n";
    //std::cerr<<(int)Rb_[0] << " "<< (int)Rb_[1] << " " << (int)Rb_[2]<< " " << (int)Rb_[3] << "\n";
    //std::cerr<< Rs.get_value(superblock_index) << "----" << Rb.get_value(block_index) << "---" << Rp << "\n";
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
    //return (Rs.size() + 6) * 64 + Rb.size() * 8;
    return Rs.get_size_in_bits() + Rb.get_size_in_bits();
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
    //std::cerr<<"arr_size " << arr_size << " length " << length << " bsize " << (int)bsize << "\n";
    for (auto& item : Rs.get_arr()) {
        fout.write((char*) &item, sizeof(item));
    }
    arr_size = Rb.get_arr().size();
    fout.write((char*) &arr_size, sizeof(arr_size));
    length = Rb.get_length();
    fout.write((char*) &length, sizeof(length));
    bsize = Rb.get_bsize();
    fout.write((char*) &bsize, sizeof(bsize));
    //std::cerr<<"arr_size " << arr_size << " length " << length << " bsize " << (int)bsize << "\n";
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
    uint64_t arr_length = static_cast<uint64_t>(num_bits/64) + 1;;
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
    //std::cerr<<"arr_size " << arr_size << " length " << length << " bsize " << (int)bsize << "\n";
    Rs = bit_vector(length, bsize); 
    for (uint64_t i = 0; i < arr_size; i++) {
        uint64_t superblock_item = 0;
	fin.read((char*)& superblock_item, sizeof(superblock_item));
	Rs.set_arr(i, superblock_item);
    }
    fin.read((char*) &arr_size, sizeof(arr_size));
    fin.read((char*) &length, sizeof(length));
    fin.read((char*) &bsize, sizeof(bsize));
    //std::cerr<<"arr_size " << arr_size << " length " << length << " bsize " << (int)bsize << "\n";
    Rb = bit_vector(length, bsize); 
    for (uint64_t i = 0; i < arr_size; i++) {
        uint64_t block_item = 0;
	fin.read((char*)& block_item, sizeof(block_item));
	Rb.set_arr(i, block_item);
    }
    fin.close();
    return true;
}
