#include "FMIndex.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr<<"Wrong number of input\n";
        std::cerr<<"Please specify the following formats:\n";
        std::cerr<<"bwocc build <input string> <output file>\n";
        std::cerr<<"bwocc query <saved fmindex> <query patterns>\n";
	return 0;
    }
    std::string command = argv[1];
    if (command == "build") {
	std::cerr<<"build\n";
	FMIndex fm_(argv[2]);
	std::cerr<<"Index is successfully built!\n";
	std::cerr<<argv[3]<<"\n";
	fm_.save(argv[3]);
    } else if(command == "query") {
        std::cerr<<"query\n";
	FMIndex fm_;
	fm_.load(argv[2]);
	std::ifstream infile(argv[3]);
	std::string pattern;
	while (infile >> pattern) {
	    uint64_t from, to;
	    fm_.query(pattern, from, to);
	    std::cerr<<from<<"\t"<<to<<"\n";
        }
    } else {	    
      std::cerr<<"Undefined command\n";
      std::cerr<<"Please specify the following formats:\n";
      std::cerr<<"bwocc build <input string> <output file>\n";
      std::cerr<<"bwocc query <saved fmindex> <query patterns>\n";
    }
    return 0;
}
