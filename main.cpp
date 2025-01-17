#include "Parser.h"
#include "RtlVisualizer.h"
#include "EmitFIRRTL.h"
#include "FIRToHW.h"
#include "ArcBackend.h"
#include "Testbench.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <csignal>

static const auto usageString =
        "Usage\n"
        "=====\n\n"
        "veripython [OPTION]... [FILE]...\n"
        "Verilog (subset) frontend and Python testbench generator with CIRCT (LLVM) backend\n\n"
        "This program supports parsing a subset of verilog and generate IR in FIRRTL. Supported arguments:\n"
        "  -o           Specify the output filename\n"
        "  -ast         Output the AST in Json format.\n"
        "  -vis         Use graphviz to generate the RTL view. Your system must support the 'dot' command\n"
        "  -token       Only output the token stream\n"
        "  -firrtl      Parse the verilog source file and emit IR in FIRRTL Dialect\n"
        "  -hw          Parse the verilog source file and emit IR in HW Dialect\n"
        "  -llvm        Use Arcilator (CIRCT) as backend to emit LLVM IR for verilog simulation\n"
        "  -testbench   [Default] Generate Python testbench module for simulation (requires llvmlite and pyvcd)\n"
        "\n"
        "Example:\n"
        "veripython full_adder.v -o full_adder.json -ast    # Emit AST in json\n"
        "veripython mux_test.v -firrtl                      # Emit FIRRTL\n"
        "veripython mux_test.v                              # Generate Python testbench module\n";

enum class FrontendTask {
    NOT_SPECIFIED,
    AST,
    TOKEN_STREAM,
    EMIT_FIRRTL,
    EMIT_HW,
    EMIT_LLVM,
    PYTHON_TESTBENCH
};

std::string getAllTokens(const std::string &filename);

static void usage() {
    std::cerr << usageString << std::endl;
}

static void signalHandler(int signal) {
    if (signal == SIGSEGV) {
        std::cerr << "Fatal internal compiler (might be circt) error occurred." << std::endl;
        exit(1);
    }
}

int main(int argc, char **argv) {
    std::signal(SIGSEGV, signalHandler);
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
        } else if (!strcmp(argv[i], "-firrtl")) {
            if (task != FrontendTask::NOT_SPECIFIED) {
                usage();
                return 1;
            }
            task = FrontendTask::EMIT_FIRRTL;
        } else if (!strcmp(argv[i], "-hw")) {
            if (task != FrontendTask::NOT_SPECIFIED) {
                usage();
                return 1;
            }
            task = FrontendTask::EMIT_HW;
        } else if (!strcmp(argv[i], "-llvm")) {
            if (task != FrontendTask::NOT_SPECIFIED) {
                usage();
                return 1;
            }
            task = FrontendTask::EMIT_LLVM;
        } else {
            inputFiles.emplace_back(argv[i]);
        }
    }

    if (task == FrontendTask::NOT_SPECIFIED) {
        task = FrontendTask::PYTHON_TESTBENCH;
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
            if (!out) {
                std::cerr << "Unable to open " << outputFileName << std::endl;
                return 1;
            }
            out << tokenStream << std::endl;
            out.close();
        }
        return 0;
    }

    try {
        auto parser = Parser(inputFiles[0]);
        parser.parseHDL();
        parser.hardwareModule.buildCircuit();
        if (visualization) {
            RtlVisualizer::visualize(parser.hardwareModule);
        }

        std::string outputData;

        if (task == FrontendTask::AST) {
            outputData = parser.hardwareModule.toString();
        } else {
            auto emitter = EmitFIRRTL{parser.hardwareModule};
            auto module = emitter.emitModuleOp();
            if (task != FrontendTask::EMIT_FIRRTL) {
                auto converter = FIRToHW(emitter.getContext());
                converter.convertToHW(module);
            }
            if (task != FrontendTask::EMIT_LLVM && task != FrontendTask::PYTHON_TESTBENCH) {
                outputData = EmitFIRRTL::ModuleToMLIR(module);
            } else {
                auto backend = ArcBackend(emitter.getContext());
                backend.convertHWToArc(module);
                if (task == FrontendTask::PYTHON_TESTBENCH) {
                    auto testbench = Testbench(module);
                    auto llvmIR = backend.convertArcToLLVMIR(module);
                    outputData = testbench.emitPythonModule(llvmIR);
                } else {
                    outputData = backend.convertArcToLLVMIR(module);
                }
            }
            module.erase();
        }

        if (outputFileName.empty()) {
            std::cout << outputData << std::endl;
        } else {
            std::ofstream out{outputFileName};
            if (!out) {
                std::cerr << "Unable to open " << outputFileName << std::endl;
                return 1;
            }
            out << outputData << std::endl;
            out.close();
        }
    } catch (const ParsingException &e) {
        std::cerr << "Parser error: " << e.what() << std::endl;
        return 1;
    } catch (const CircuitException &e) {
        std::cerr << "Internal circuit error: " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
