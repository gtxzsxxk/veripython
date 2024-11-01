#include "Parser.h"
#include <iostream>

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cerr << "Bad argument" << std::endl;
    }
    auto parser = Parser(argv[1]);
    parser.doParse();
    return 0;
}
