//
// Created by hanyuan on 2024/11/2.
//
#include "../Parser.h"
#include "../RtlSimulator.h"
#include "../EmitFIRRTL.h"
#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>

int evaluateExpression(const std::string &expression) {
    std::string command = "echo $(( " + expression + " ))";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command\n";
        return 0;
    }

    int result;
    fscanf(pipe, "%d", &result);
    pclose(pipe);
    return result;
}

std::string generateComplexExpressionNoParen(int depth) {
    if (depth == 0) {
        return std::to_string((rand() % 50) + 1);
    }

    std::ostringstream expression;
    remake:
    expression.clear();
    expression << generateComplexExpressionNoParen(depth - 1) << " ";

    char operators[] = {'+', '-', '*', '/'};
    char op = operators[rand() % 4]; // 随机选择一个操作符
    std::string rhsExpr = generateComplexExpressionNoParen(depth - 1);
    if (op == '/') {
        while (evaluateExpression(rhsExpr) == 0) {
            rhsExpr = generateComplexExpressionNoParen(depth - 1);
        }
    }
    expression << op << " " << rhsExpr;

    std::string answer = expression.str();
    if (evaluateExpression(answer) == 0) {
        goto remake;
    }

    return answer;
}

std::string generateComplexExpressionWithParen(int depth) {
    if (depth == 0) {
        return std::to_string((rand() % 50) + 1);
    }

    std::ostringstream expression;
    remake:
    expression.str("");
    expression.clear();
    std::string lhsExpr = generateComplexExpressionWithParen(depth - 1);

    if (rand() % 2 == 0) {
        expression << lhsExpr << " ";
    } else {
        expression << "(" << lhsExpr << ") ";
    }

    char operators[] = {'+', '-', '*', '/'};
    char op = operators[rand() % 4]; // 随机选择一个操作符
    std::string rhsExpr = generateComplexExpressionWithParen(depth - 1);
    if (op == '/') {
        while (evaluateExpression(rhsExpr) == 0) {
            rhsExpr = generateComplexExpressionWithParen(depth - 1);
        }
    }
    expression << op << " ";

    if (rand() % 2 == 0) {
        expression << rhsExpr;
    } else {
        expression << "(" << rhsExpr << ")";
    }

    std::string answer = expression.str();
    if (evaluateExpression(answer) == 0) {
        goto remake;
    }

    return answer;
}

uint64_t circuitDataToUInt(const CircuitData &data) {
    uint64_t ret = 0;
    if (data.bits.empty()) {
        throw std::runtime_error("Cannot convert a circuit data to unsigned integer");
    }
    for (auto i = (ssize_t) data.bits.size() - 1; i >= 0; i--) {
        ret <<= 1;
        if (data.bits[i] == -1) {
            throw std::runtime_error("Has unknown bits, cannot convert");
        }
        ret |= data.bits[i];
    }
    return ret;
}

TEST(ParserTests, SimpleConstantExpressionParsing) {
    const std::string filename = "expressions.v";
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < 10; i++) {
        std::ofstream outFile(filename);
        auto expressionString = generateComplexExpressionNoParen((rand() % 6) + 1);
        std::cout << "Testing: " << expressionString << std::endl;
        int realValue = evaluateExpression(expressionString);
        outFile << expressionString << std::endl;
        outFile.close();

        auto parser = Parser(filename);
        auto ast = parser.parseConstantExpr();
        int parserValue = ast->eval();
        EXPECT_EQ(parserValue, realValue);
        std::remove(filename.c_str());
    }
}

TEST(ParserTests, HaveParenConstantExpressionParsing) {
    const std::string filename = "expressions.v";
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < 10; i++) {
        std::ofstream outFile(filename);
        auto expressionString = generateComplexExpressionWithParen((rand() % 4) + 1);
        std::cout << "Testing: " << expressionString << std::endl;
        int realValue = evaluateExpression(expressionString);
        outFile << expressionString << std::endl;
        outFile.close();

        auto parser = Parser(filename);
        auto ast = parser.parseConstantExpr();
        int parserValue = ast->eval();
        EXPECT_EQ(parserValue, realValue);
        std::remove(filename.c_str());
    }
}

TEST(ParserTests, SimpleSingleModuleVerilogSimTest) {
    const std::string filename = "../tests/verilog_srcs/full_adder.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto simulator = RtlSimulator{parser.hardwareModule};

    auto aData = CircuitData(PortSlicingAST(0, 0));
    aData.bits[0] = true;
    auto bData = CircuitData(PortSlicingAST(0, 0));
    bData.bits[0] = true;
    auto ciData = CircuitData(PortSlicingAST(0, 0));
    ciData.bits[0] = false;

    simulator.poke("a", aData);
    simulator.poke("b", bData);
    simulator.poke("carryin", ciData);
    simulator.printOutcome();
    EXPECT_EQ(simulator.peek("y").bits[0], 0);
    EXPECT_EQ(simulator.peek("carryout").bits[0], 1);
}

TEST(ParserTests, SimpleSlicingSimTest) {
    const std::string filename = "../tests/verilog_srcs/slicing_test.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto simulator = RtlSimulator{parser.hardwareModule};

    auto aData = CircuitData(PortSlicingAST(2, 0));
    aData.bits[0] = true;
    aData.bits[1] = false;
    aData.bits[2] = true;
    auto bData = CircuitData(PortSlicingAST(2, 0));
    bData.bits[0] = true;
    bData.bits[1] = false;
    bData.bits[2] = true;

    simulator.poke("a", aData);
    simulator.poke("b", bData);
    EXPECT_EQ(simulator.peek("c").bits[0], -1);
    EXPECT_EQ(simulator.peek("c").bits[1], -1);
    EXPECT_EQ(simulator.peek("c").bits[2], 1);
    EXPECT_EQ(simulator.peek("c").bits[3], 0);
    EXPECT_EQ(simulator.peek("c").bits[4], 0);
    EXPECT_EQ(simulator.peek("c").bits[5], -1);
}

TEST(ParserTests, SimpleMuxSimTest) {
    const std::string filename = "../tests/verilog_srcs/mux_test.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto simulator = RtlSimulator{parser.hardwareModule};

    auto aData = CircuitData(PortSlicingAST(2, 0));
    aData.bits[0] = true;
    aData.bits[1] = true;
    aData.bits[2] = true;
    auto bData = CircuitData(PortSlicingAST(3, 0));
    bData.bits[0] = true;
    bData.bits[1] = false;
    bData.bits[2] = true;
    bData.bits[3] = true;
    auto selData = CircuitData(PortSlicingAST(1, 0));
    selData.bits[0] = true;
    selData.bits[1] = false;

    simulator.poke("a", aData);
    simulator.poke("b", bData);
    simulator.poke("sel", selData);
    EXPECT_EQ(simulator.peek("out").bits[0], 0);
    EXPECT_EQ(simulator.peek("out").bits[1], 1);
    EXPECT_EQ(simulator.peek("out").bits[2], 1);
}

TEST(ParserTests, SimpleConcatSimTest) {
    const std::string filename = "../tests/verilog_srcs/concat.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto simulator = RtlSimulator{parser.hardwareModule};

    auto aData = CircuitData(PortSlicingAST(3, 0));
    aData.bits[0] = true;
    aData.bits[1] = true;
    aData.bits[2] = true;
    aData.bits[3] = false;
    auto bData = CircuitData(PortSlicingAST(3, 0));
    bData.bits[0] = false;
    bData.bits[1] = true;
    bData.bits[2] = true;
    bData.bits[3] = true;
    auto singleBit = CircuitData(PortSlicingAST(0, 0));
    singleBit.bits[0] = true;

    simulator.poke("a", aData);
    simulator.poke("b", bData);
    simulator.poke("single_bit", singleBit);
    EXPECT_EQ(simulator.peek("result1").bits[0], -1);

    EXPECT_EQ(simulator.peek("result2_1").bits[0], 1);
    EXPECT_EQ(simulator.peek("result2_1").bits[1], 0);

    EXPECT_EQ(simulator.peek("result3_0").bits[0], 1);
    EXPECT_EQ(simulator.peek("result3_0").bits[1], 1);
    EXPECT_EQ(simulator.peek("result3_0").bits[2], -1);

    EXPECT_EQ(simulator.peek("result2_0").bits[0], 1);
    EXPECT_EQ(simulator.peek("result2_0").bits[1], 1);
    EXPECT_EQ(simulator.peek("result2_0").bits[2], 1);
    EXPECT_EQ(simulator.peek("result2_0").bits[3], 0);
}

TEST(ParserTests, SimpleRegSimTest) {
    const std::string filename = "../tests/verilog_srcs/reg_simple_test0.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto simulator = RtlSimulator{parser.hardwareModule};
    auto clkData = CircuitData(PortSlicingAST(0, 0));
    auto resetData = CircuitData(PortSlicingAST(0, 0));

    resetData.bits[0] = true;
    simulator.poke("rst_n", resetData);
    clkData.bits[0] = false;
    simulator.poke("clk", clkData);
    simulator.printOutcome();

    resetData.bits[0] = false;
    simulator.poke("rst_n", resetData);
    clkData.bits[0] = true;
    simulator.poke("clk", clkData);
    clkData.bits[0] = false;
    simulator.poke("clk", clkData);
    EXPECT_EQ(0, circuitDataToUInt(simulator.peek("count")));
    simulator.printOutcome();

    resetData.bits[0] = true;
    simulator.poke("rst_n", resetData);
    EXPECT_EQ(0, circuitDataToUInt(simulator.peek("count")));
    simulator.printOutcome();

    for (int i = 1; i <= 255; i++) {
        clkData.bits[0] = false;
        simulator.poke("clk", clkData);
        clkData.bits[0] = true;
        simulator.poke("clk", clkData);
        EXPECT_EQ(i, circuitDataToUInt(simulator.peek("count")));
    }

    clkData.bits[0] = false;
    simulator.poke("clk", clkData);
    clkData.bits[0] = true;
    simulator.poke("clk", clkData);
    EXPECT_EQ(0, circuitDataToUInt(simulator.peek("count")));
}

TEST(ParserTests, LLVMTest) {
    const std::string filename = "../tests/verilog_srcs/mux_test.v";

    auto parser = Parser(filename);
    parser.parseHDL();
    parser.hardwareModule.buildCircuit();
    auto emitter = EmitFIRRTL(parser.hardwareModule);
    std::cout << emitter.emit() << std::endl;
}
