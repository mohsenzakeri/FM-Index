#include "FMIndex.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr<<"Wrong number of input\n";
	return 0;
    }
    std::string command = argv[1];
    if (command == "build") {
	std::cerr<<"build\n";
	FMIndex fm_(argv[2]);
	std::cerr<<argv[3]<<"\n";
	fm_.save(argv[3]);
    } else if(command == "query") {
        std::cerr<<"query\n";
    } else {	    
      std::cerr<<"Undefined command\n";
    }
    return 0;
}
