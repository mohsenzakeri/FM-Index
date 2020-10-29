#include <cstring>
#include <map>

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
    for (uint64_t i = 0; i < length; i++) {
        uint64_t char_index = static_cast<uint64_t>(input_text[i]);
	all_chars[char_index] += 1;
    }
    std::map<unsigned char, uint32_t> alphamap;
    uint32_t alphabet_index = 0;
    for (uint64_t i = 0; i < all_chars_num; i++) {
        if (all_chars[i] != 0) {
	    alphabet.push_back(static_cast<unsigned char>(i)); 
	    counts.push_back(all_chars[i]);
	    occs.push_back(new bit_vector(length));
	    alphamap[static_cast<unsigned char>(i)] = alphabet_index;
	    alphabet_index +=1 ;
	}
    }
    for (uint64_t i = 0; i < alphabet.size(); i++)
	    std::cerr<< i << " " << alphabet[i] << " " << counts[i] << "\n";

    for (uint64_t i = 0; i < length; i++) {
        uint32_t alphabet_index = alphamap[bwt[i]];
	occs[alphabet_index]->set_index(i);
    }
    for (uint64_t i = 0; i < occs.size(); i++) {
	std::cerr<<i<<":";
        occs[i]->show();
    }
}


void FMIndex::save(char* output_dir) {

}
