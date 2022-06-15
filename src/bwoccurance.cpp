#include "FMIndex.hpp"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr<<"Wrong number of input "<< argc << "\n";
        std::cerr<<"Please specify the following formats:\n";
        std::cerr<<"bwocc build <input string> <output file>\n";
        std::cerr<<"bwocc query <saved fmindex> <query patterns>\n";
	    return 0;
    }
    std::string command = argv[1];
    if (command == "build") {
	    std::cerr<<"build\n";
        std::string type = argv[2];
        if (type == "move") {
            std::cerr<<"The move structure is being built.\n";
    	    FMIndex fm_(argv[3], true);
	        std::cerr<<"Index is successfully built!\n";
            std::cerr<<"query:\n";
            std::string R = "GAAGTGCATGGGAACTTCACTC";
            std::vector<uint32_t> ms_lens;
            fm_.move_rl_query_ms(R, ms_lens);
            std::cerr<<"matching statistics:\n";
            for (auto& len: ms_lens) {
                std::cerr<<len<<"\n";
            }
        } else if (type == "regular") {
            std::cerr<<"The regular FM index is being built.\n";
    	    FMIndex fm_(argv[3], false);
	        std::cerr<<"Index is successfully built!\n";
	        std::cerr<<argv[3]<<"\n";
	        fm_.save(argv[4]);
        } else {
            std::cerr<<"The indicated type ("
                    << type << ") is not known."
                    << " Please choose from {move, regular}\n";
        }
    } else if(command == "query") {
        std::cerr<<"query\n";
	    FMIndex fm_;
	    fm_.load(argv[3]);
	    std::ifstream infile(argv[4]);
	    std::string pattern;
        std::cerr<<"before while query!\n";
	    while (infile >> pattern) {
	        uint64_t from, to;
	        fm_.query(pattern, from, to);
	        std::cerr<<from<<"\t"<<to<<"\n";
        }
        std::cerr<<"after while query!\n";
    } else {	    
      std::cerr<<"Undefined command\n";
      std::cerr<<"Please specify the following formats:\n";
      std::cerr<<"bwocc build <index type> <input string> <output file>\n";
      std::cerr<<"bwocc query <saved fmindex> <query patterns>\n";
    }
    return 0;
}
