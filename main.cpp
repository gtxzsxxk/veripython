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
                         "  -ast         Output the AST in XML format.\n"
                         "  -vis         Use graphviz to generate the RTL view. Your system must support the 'dot' command\n"
                         "\n"
                         "Example:\n"
                         "veripython full_adder.v -o full_adder.xml -ast\n";

enum class FrontendTask {
    NOT_SPECIFIED,
    AST,
};

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
            task = FrontendTask::AST;
        } else if (!strcmp(argv[i], "-vis")) {
            visualization = true;
        } else {
            inputFiles.emplace_back(argv[i]);
        }
    }

    if (inputFiles.empty() || inputFiles.size() > 1) {
        usage();
        return 1;
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
