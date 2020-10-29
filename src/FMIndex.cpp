#include <cstring>
#include <map>
#include <sys/stat.h> 

#include "FMIndex.hpp"

FMIndex::FMIndex(char* input_file) {
    // the code for reading a file to a char* is 
    // borrowed from https://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c
    FILE *f = fopen(static_cast<char*>(input_file), "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *input_text = (char *)malloc(fsize + 1);
    fread(input_text, fsize, 1, f);
    fclose(f);

    input_text[fsize] = 0;

    build(input_text);
    std::cerr<<input_text<<" done!\n";
}

void FMIndex::build(char* input_text) {
    length = strlen(input_text);
    int *A = (int *)malloc(length * sizeof(int));
    bwt = (unsigned char*)malloc(length * sizeof(unsigned char));
    divbwt((unsigned char *)input_text, bwt, A, length);
    std::cerr<<"bwt:"<<bwt<<"\n";

    uint32_t all_chars_num = 256;
    std::vector<uint64_t> all_chars = std::vector<uint64_t>(all_chars_num, 0);
    for (uint64_t i = 0; i < length-1; i++) {
        uint64_t char_index = static_cast<uint64_t>(input_text[i]);
	all_chars[char_index] += 1;
    }
    std::map<unsigned char, uint32_t> alphamap;
    uint32_t alphabet_index = 0;
    for (uint64_t i = 0; i < all_chars_num; i++) {
        if (all_chars[i] != 0) {
	    alphabet.push_back(static_cast<unsigned char>(i)); 
	    counts.push_back(all_chars[i]);
	    bit_vector* new_bit_vector = new bit_vector(length);
	    occs_rank.push_back(new rank_support(new_bit_vector));
	    alphamap[static_cast<unsigned char>(i)] = alphabet_index;
	    alphabet_index +=1 ;
	}
    }
    for (uint64_t i = 0; i < alphabet.size(); i++)
	    std::cerr<< i << " " << alphabet[i] << " " << counts[i] << "\n";

    for (uint64_t i = 0; i < length-1; i++) {
        uint32_t alphabet_index = alphamap[bwt[i]];
	occs_rank[alphabet_index]->get_bit_vec()->set_index(i);
    }
}


void FMIndex::save(char* output_dir) {
    int check = mkdir(output_dir,0777);
    if (!check) 
        std::cerr<<"Directory created\n"; 
    else {
        std::cerr<<"Unable to create directory\n"; 
    } 
    uint64_t index = 0;
    for (auto& occ_rank : occs_rank) {
	std::string curr_char(1, alphabet[index]);
	std::string file_name = static_cast<std::string>(output_dir) + "/" + curr_char + ".rank";
        occ_rank->save(file_name);
	index +=1;
    }
    std::string fname = static_cast<std::string>(output_dir) + "/index.bin";
    std::ofstream fout(fname, std::ios::out | std::ios::binary);
    fout.write((char*) &length, sizeof(length));
    for(uint64_t i = 0; i < length; i++) {
	unsigned char curr_char = bwt[i];
        fout.write((char*) &curr_char, sizeof(curr_char));
    }
    uint32_t alphabet_size = counts.size();
    fout.write((char*) &alphabet_size, sizeof(alphabet_size));
    for (auto& item : counts)
        fout.write((char*) &item, sizeof(item));
    for (auto& item : alphabet)
	fout.write((char*) &item, sizeof(item));
    fout.close();
}

void FMIndex::load(char* index_dir) {
    std::string fname = static_cast<std::string>(index_dir) + "/index.bin";
    std::ifstream fin(fname, std::ios::in | std::ios::binary);

    fin.read((char*) &length, sizeof(length));
    bwt = (unsigned char*)malloc(length * sizeof(unsigned char));
    unsigned char curr_char;
    for (uint64_t i = 0; i < length; i++) {
        fin.read((char*) &curr_char, sizeof(curr_char));
	bwt[i] = curr_char;
    }
    uint32_t alphabet_size = 0;
    fin.read((char*) &alphabet_size, sizeof(alphabet_size));
    uint64_t count = 0;
    for (uint32_t i = 0; i < alphabet_size; i++) {
        fin.read((char*) &count, sizeof(count));
	counts.push_back(count);
    }
    unsigned char curr_alpha = 0;
    for (uint32_t i = 0; i < alphabet_size; i++) {
        fin.read((char*) &curr_alpha, sizeof(curr_alpha));    
	alphabet.push_back(curr_alpha);
    }
    fin.close();
    for (uint32_t i = 0; i < alphabet_size; i++) {
        std::string curr_char(1, alphabet[i]);
	std::string file_name = static_cast<std::string>(index_dir) + "/" + curr_char + ".rank";
        rank_support* new_rank = new rank_support();
	new_rank->load(file_name);
	occs_rank.push_back(new_rank);
    }
    std::cerr<<"bwt:"<<bwt<<" loaded\n";
}
