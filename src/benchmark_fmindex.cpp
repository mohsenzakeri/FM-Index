#include <cstring>
#include <sys/stat.h> 
#include <algorithm>
#include <chrono>

#include "FMIndex.hpp"

int main() {
    FMIndex fm;

    fm.load("/home/mohsen/Desktop/viral/NC_042013.fmindex/"); 

    FILE *f = fopen(static_cast<char*>("/home/mohsen/Desktop/viral/NC_042013.fasta"), "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *input_text = static_cast<char *>(malloc(fsize + 1));
    fread(input_text, fsize, 1, f);
    fclose(f);
    input_text[fsize] = 0;
    std::string input_string(input_text);
    input_string.erase(std::remove(input_string.begin(), input_string.end(), '\n'),
            input_string.end());
    delete input_text;
    input_text = static_cast<char *>(malloc(input_string.size() + 1));
    strcpy(static_cast<char*>(input_text), input_string.c_str());
    std::cerr<<fsize<< " " << strlen(input_text)<<"\n";

    for (int j = 1000; j<1000001; j*=10) {
	    std::vector<std::string> all_patterns;
        for (int i = 0; i <j; i++) {
	    int32_t len = input_string.size();
	    std::string pattern = input_string.substr(i%(len-100), 50);
	    //if (i%5<1)
	    //  std::reverse(pattern.begin(), pattern.end());
	    all_patterns.push_back(pattern);
        }
        uint64_t from, to;
    	auto t1 = std::chrono::high_resolution_clock::now();
        for (auto& pattern : all_patterns)
            fm.query(pattern, from, to);
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cerr<<j << "\t" << duration1 << "\n";
    }
}
