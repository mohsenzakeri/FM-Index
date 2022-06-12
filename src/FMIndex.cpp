#include <cstring>
#include <sys/stat.h> 
#include <algorithm>

#include "FMIndex.hpp"

FMIndex::FMIndex(char* input_file) {
    // the code for reading a file to a char* is 
    // borrowed from https://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c
    FILE *f = fopen(static_cast<char*>(input_file), "rb");
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
    build(input_text);
}

void FMIndex::build(char* input_text) {
    length = strlen(input_text);
    int *A = (int *)malloc((length+1) * sizeof(int));
    bwt = (unsigned char*)malloc((length+1) * sizeof(unsigned char));
    int *SA = (int *)malloc(length * sizeof(int));
    divsufsort((unsigned char *)input_text, SA, length);
    bwt[0] = input_text[length-1];
    for(int i = 0; i < length; ++i) {
	if (SA[i] == 0)
            bwt[i+1] = '$';
	else
            bwt[i+1] = input_text[SA[i] - 1];
    }
    free(SA);
    //std::cerr<<"bwt:"<<bwt<<"\n";
    char last_char = input_text[length-1];
    uint32_t all_chars_num = 256;
    std::vector<uint64_t> all_chars = std::vector<uint64_t>(all_chars_num, 0);
    for (uint64_t i = 0; i <= length; i++) {
        uint64_t char_index = static_cast<uint64_t>(bwt[i]);
	if (input_text[i] == '\n')
	    continue;
	all_chars[char_index] += 1;
    }
    uint32_t alphabet_index = 0;
    for (uint64_t i = 0; i < all_chars_num; i++) {
        if (all_chars[i] != 0) {
	    alphabet.push_back(static_cast<unsigned char>(i)); 
	    counts.push_back(all_chars[i]);
	    bit_vector* new_bit_vector = new bit_vector(length+1);
	    occs.push_back(new_bit_vector);
	    //occs_rank.push_back(new rank_support(new_bit_vector));
	    alphamap[static_cast<unsigned char>(i)] = alphabet_index;
	    alphabet_index +=1 ;
	}
    }
    for (uint64_t i = 0; i < alphabet.size(); i++)
        std::cerr<< i << " " << alphabet[i] << " " << counts[i] << "\n";

    for (uint64_t i = 0; i < length + 1; i++) {
        uint32_t alphabet_index = alphamap[bwt[i]];
	occs[alphabet_index]->set_index(i);
	
    }
    for (auto& item: occs) {
        occs_rank.push_back(new rank_support(item));
    }
}


uint32_t FMIndex::LF(uint32_t row) {
    uint32_t lf = 1;
    uint32_t i = alphamap[bwt[row]];
    for (uint32_t k = 0; k < i; k++) {
        lf += counts[k];
    }
    for (uint32_t k = 0; k < row; k++) {
        if (bwt[k] == bwt[row])
            lf += 1;
    }
    return lf;
}

void FMIndex::build_move(char* input_text) {
    length = strlen(input_text);
    int *A = (int *)malloc((length+1) * sizeof(int));
    bwt = (unsigned char*)malloc((length+1) * sizeof(unsigned char));
    SA = (int *)malloc(length * sizeof(int));
    divsufsort((unsigned char *)input_text, SA, length);
    bwt[0] = input_text[length-1];
    for(int i = 0; i < length; ++i) {
        if (SA[i] == 0)
                bwt[i+1] = '$';
        else
                bwt[i+1] = input_text[SA[i] - 1];
    }
    // free(SA);

    char last_char = input_text[length-1];
    uint32_t all_chars_num = 256;
    std::vector<uint64_t> all_chars = std::vector<uint64_t>(all_chars_num, 0);
    for (uint64_t i = 0; i <= length; i++) {
        uint64_t char_index = static_cast<uint64_t>(bwt[i]);
        if (input_text[i] == '\n')
            continue;
        all_chars[char_index] += 1;
    }
    uint32_t alphabet_index = 0;
    for (uint64_t i = 0; i < all_chars_num; i++) {
        if (all_chars[i] != 0) {
            alphabet.push_back(static_cast<unsigned char>(i)); 
            counts.push_back(all_chars[i]);
            alphamap[static_cast<unsigned char>(i)] = alphabet_index;
            alphabet_index +=1 ;
    	}
    }

    uint32_t len = 1;
    uint32_t offset = 0;
    bit_vector* bits = new bit_vector(length+1);
    uint32_t i = 0;
    for (;i < length - 1; i++) {
        if (bwt[i] == bwt[i+1])
            len += 1;
        else {
            uint32_t lf  = 0;
            if (bwt[i] != '$')
                lf = LF(offset);
            move_row row = move_row(offset, length, lf, i, bwt[i]);
            bits->set_index(offset);
            rlbwt.push_back(row);
            offset += len;
            len = 1;
        }
    }

    uint32_t    lf  = 0;
    if (bwt[i] != '$')
        lf = LF(offset);
    move_row row = move_row(offset, length, lf, i, bwt[i]);
    rlbwt.push_back(row);
    bits->set_index(offset);
    // print(bits)
    rank_support rbits = rank_support(bits);
    for (auto& row: rlbwt) {
        uint32_t set_bit = row.pp;
        while (set_bit > 0 and bits->read_bit(set_bit) == 0)
            set_bit -= 1;
        if (set_bit == 0 and bits->read_bit(set_bit) == 0)
            row.id = 0;
        else
            row.id = rbits.rank1(set_bit) - 1;
    }
}

int32_t FMIndex::get_index(uint32_t i) {
    int32_t index = 0;
    while (i >= rlbwt[index].p + rlbwt[index].n)
        index += 1;
    return index - 1;
}

int32_t FMIndex::fast_forward(uint32_t pointer, uint32_t index) {
    while (index < rlbwt.size() and pointer >= rlbwt[index].p + rlbwt[index].n)
        index += 1;
    if (index >= rlbwt.size())
        return -1;
    return index;
}

uint32_t LCP(std::string x, std::string y) {
    uint32_t lcp = 0;
    for (uint32_t i = 0; i < std::min(x.length(), y.length()); i++) {
        if (x[i] == y[i])
            lcp += 1;
        else
            break;
    }
    return lcp;
}

int32_t FMIndex::move_rl_query_ms(std::string R, std::string S, std::vector<uint32_t>& ms_lens) {
    uint32_t pointer = 1;
    uint32_t pos_on_r = R.length() - 1;
    uint32_t i = alphamap[R[pos_on_r]];
    for (uint32_t k = 0; k < i; k++)
        pointer += counts[k];
    std::cout<< "Suffix Array[" << pointer << "] = " << SA[pointer] << "\n";
    if (R[pos_on_r] == '0' or R[pos_on_r] == '1')
        while (SA[pointer] % 2 == 0 and pointer < S.length())
            pointer += 1;
    int32_t index = get_index(pointer);
    uint32_t match_len = 1;
    while (true) {
        pos_on_r -= 1;
        std::cout << pointer << " - match_len: " << match_len << " pos_on_r: " << pos_on_r << "\n";
        if (pos_on_r == -1) {
            std::cerr << "Found! at position " << SA[pointer] << "\n";
            ms_lens.push_back(match_len);
            return 1;
        }
        auto& row = rlbwt[index];
        if (row.c == R[pos_on_r]) {
            pointer = row.pp + (pointer - row.p);
            index = row.id;
            index = fast_forward(pointer, index);
            if (index == -1)
                return 1;
            match_len += 1;
            continue;
        }

        int32_t q_ = -1;
        int32_t q__ = -1;
        int32_t k_ = -1;
        int32_t k__ = -1;

        // finding the last preceeding match
        for (int32_t k = index - 1; k > -1; k--) {
            if (rlbwt[k].c == R[pos_on_r]) {
                q_ = rlbwt[k].p + rlbwt[k].n - 1;
                k_ = k;
                break;
            }
        }
        // finding the first proceeding match
        for (uint32_t k = index + 1; k < rlbwt.size(); k++) {
            if (rlbwt[k].c == R[pos_on_r]) {
                q__ = rlbwt[k].p;
                k__ = k;
                break;
            }
        }
        // If no match was found, then the pattern doesn't match at all.
        if (q_ == -1 and q__ == -1) {
            ms_lens.push_back(match_len);
            i = alphamap[R[pos_on_r]];
            pointer = 1;
            for (uint32_t k = 0; k < i; k++)
                pointer += counts[k];
            index = get_index(pointer);
            if (index == -1)
                return 1;
            match_len = 1;
            continue;
        }  
        // computing LCPs for the preceeding and proceeding matches
        auto q_s = S.substr(SA[q_], S.length() - SA[q_]);
        auto q__s = S.substr(SA[q__], S.length() - SA[q__]);
        auto pointer_s = S.substr(SA[pointer], S.length() - SA[pointer]);
        if (LCP(pointer_s, q_s) >= LCP(pointer_s, q__s) and LCP(pointer_s, q_s) >= match_len) {
            row = rlbwt[k_];
            pointer = row.pp + (q_ - row.p);
            index = row.id;
            index = fast_forward(pointer, index);
            match_len += 1;
            continue;
        }
        else if (LCP(pointer_s, q__s) >= match_len) {
            row = rlbwt[k__];
            pointer = row.pp + (q__ - row.p);
            index = row.id;
            index = fast_forward(pointer, index);
            match_len += 1;
            continue;
        }
        else {
            ms_lens.push_back(match_len);
            i = alphamap[R[pos_on_r]];
            pointer = 1;
            for (uint32_t k = 0; k < i; k++)
                pointer += counts[k];
            index = get_index(pointer);
            if (index == -1)
                return 1;
            match_len = 1;
            continue;
        }
    }
    return -1;
}

void FMIndex::save(char* output_dir) {
    mkdir(output_dir,0777);
    uint64_t index = 0;
    for (auto& occ_rank : occs_rank) {
	std::string curr_char(1, alphabet[index]);
	std::string file_name = static_cast<std::string>(output_dir) + "/" + curr_char + ".rank";
        occ_rank->save(file_name);
	index += 1;
    }
    std::cerr<<"Rank bit vectors are stored.\n";
    std::string fname = static_cast<std::string>(output_dir) + "/index.bin";
    std::ofstream fout(fname, std::ios::out | std::ios::binary);
    fout.write((char*) &length, sizeof(length));
    for(uint64_t i = 0; i < length + 1; i++) {
	unsigned char curr_char = bwt[i];
        fout.write((char*) &curr_char, sizeof(curr_char));
    }
    uint32_t alphabet_size = counts.size();
    fout.write((char*) &alphabet_size, sizeof(alphabet_size));
    for (uint32_t i = 0; i < counts.size(); i++) {
	auto& item = counts[i];
        fout.write((char*) &item, sizeof(item));
    }
    for (uint32_t i = 0; i < alphabet.size(); i++) {
	unsigned char item = alphabet[i];
	fout.write((char*) &item, sizeof(item));
    }
    std::cerr<<"Rest of the index files are stored.\nIndex saved in "<<output_dir<<"\n";
    fout.close();
}

void FMIndex::load(char* index_dir) {
    std::string fname = static_cast<std::string>(index_dir) + "/index.bin";
    std::ifstream fin(fname, std::ios::in | std::ios::binary);

    fin.read((char*) &length, sizeof(length));
    bwt = (unsigned char*)malloc((length+1) * sizeof(unsigned char));
    unsigned char curr_char;
    for (uint64_t i = 0; i < length+1; i++) {
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

    for (uint32_t i = 0; i < alphabet_size; i++) {
        fin.read(( char*) &curr_char, sizeof(curr_char));    
	alphabet.push_back(curr_char);
	alphamap[curr_char] = i;
    }

    fin.close();

    for (uint32_t i = 0; i < alphabet_size; i++) {
        std::string curr_char(1, alphabet[i]);
	std::string file_name = static_cast<std::string>(index_dir) + "/" + curr_char + ".rank";
        rank_support* new_rank = new rank_support();
	new_rank->load(file_name);
	occs_rank.push_back(new_rank);
    }
}

bool FMIndex::query(std::string pattern, uint64_t& from_, uint64_t& to_) {
    uint64_t from = 0, to = 0;
    uint64_t* res;
    for (uint64_t i = pattern.length()-1; i > 0; i--) {
       if (i == pattern.length()-1) {
           uint64_t char_index = alphamap[pattern[i]];
	   for (uint32_t j = 0; j < char_index; j++) {
               from += counts[j];
	       to += counts[j];
	   }
	   to += counts[char_index];
       }
       res = check_next(from, to, pattern[i-1]);
       from = res[0];
       to = res[1];
       if (res[0] == res[1] or res[1] == std::numeric_limits<uint64_t>::max() or res[0] == std::numeric_limits<uint64_t>::max()) {
	   from_ = 0;
	   to_ = 0;
	   return false;
       }
    }
    from_ = res[0];
    to_ = res[1];
    return true;
}

uint64_t* FMIndex::check_next(uint64_t from, uint64_t to, char c) {
    uint64_t from_ = std::numeric_limits<uint64_t>::max(), to_ = 0;
    uint64_t to_index = 0;
    uint32_t char_index = alphamap[c];

    for (uint64_t i = from; i < to; i++) {
        if (bwt[i] == c and from_ == std::numeric_limits<uint64_t>::max()) {
            from_ = 0;
	    to_ = 0;
            for(uint32_t j = 0; j < char_index; j++) {
                from_ += counts[j];
                to_ += counts[j];
            }
	    from_ += occs_rank[char_index]->rank1(i) - 1;
	}
	if (bwt[i] == c) {
	    to_index = i;
	}
    }
    to_ += occs_rank[alphamap[c]]->rank1(to_index);
    uint64_t* res = new uint64_t[2];
    res[0] = from_;
    res[1] = to_;
    return res;
}

