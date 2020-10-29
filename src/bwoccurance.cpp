#include "FMIndex.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr<<"Wrong number of input\n";
	return 0;
    }
    std::string command = argv[1];
    if (command == "build") {
	std::cerr<<"build\n";
	//std::string input_file = argv[2];
	//FMIndex fm(input_file);
	FMIndex fm_(argv[2]);
    } else if(command == "query") {
        std::cerr<<"query\n";
    } else {	    
      std::cerr<<"Undefined command\n";
    }
    return 0;
}
