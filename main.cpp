#include "Parser.h"
#include "RtlVisualizer.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

const auto usageString = "Usage\n"
                         "=====\n\n"
                         "veripython [OPTION]... [FILE]...\n"
                         "Another implementation for Verilator with Python target\n\n"
                         "Currently only the frontend is implemented. Supported arguments:\n"
                         "  -o           Specify the output filename\n"
                         "  -ast         Output the AST in Json format.\n"
                         "  -vis         Use graphviz to generate the RTL view. Your system must support the 'dot' command\n"
                         "  -token       Only output the token stream\n"
                         "\n"
                         "Example:\n"
                         "veripython full_adder.v -o full_adder.xml -ast\n";

enum class FrontendTask {
    NOT_SPECIFIED,
    AST,
    TOKEN_STREAM
};

std::string getAllTokens(const std::string &filename);

void usage() {
    std::cerr << usageString << std::endl;
}

int main(int argc, char **argv) {
    std::string outputFileName;
    std::vector<std::string> inputFiles;
    auto task = FrontendTask::NOT_SPECIFIED;
    auto visualization = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            if (i == argc - 1) {
                usage();
                return 1;
            }
            outputFileName = std::string{argv[++i]};
        } else if (!strcmp(argv[i], "-ast")) {
            if (task != FrontendTask::NOT_SPECIFIED) {
                usage();
                return 1;
            }
            task = FrontendTask::AST;
        } else if (!strcmp(argv[i], "-vis")) {
            visualization = true;
        } else if (!strcmp(argv[i], "-token")) {
            if (task != FrontendTask::NOT_SPECIFIED) {
                usage();
                return 1;
            }
            task = FrontendTask::TOKEN_STREAM;
        } else {
            inputFiles.emplace_back(argv[i]);
        }
    }

    if (inputFiles.empty() || inputFiles.size() > 1) {
        usage();
        return 1;
    }

    if (task == FrontendTask::TOKEN_STREAM) {
        auto tokenStream = getAllTokens(inputFiles[0]);
        if (outputFileName.empty()) {
            std::cout << tokenStream << std::endl;
        } else {
            std::ofstream out{outputFileName};
            out << tokenStream << std::endl;
            out.close();
        }

        return 0;
    }

    auto parser = Parser(inputFiles[0]);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    if (visualization) {
        RtlVisualizer::visualize(parser.hardwareModule);
    }

    if (task == FrontendTask::AST) {
        auto astInXml = parser.hardwareModule.toString();
        if (outputFileName.empty()) {
            std::cout << astInXml << std::endl;
        } else {
            std::ofstream out{outputFileName};
            out << astInXml << std::endl;
            out.close();
        }
    } else {
        usage();
        return 1;
    }

    return 0;
}
