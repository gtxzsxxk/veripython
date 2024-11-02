//
// Created by hanyuan on 2024/11/2.
//
#include "../Parser.h"
#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>

std::string generateComplexExpression(int depth) {
    if (depth == 0) {
        return std::to_string(rand() % 100); // 生成0到99的随机数
    }

    std::ostringstream expression;
    expression << generateComplexExpression(depth - 1) << " ";

    char operators[] = {'+', '-', '*', '/'};
    char op = operators[rand() % 4]; // 随机选择一个操作符
    expression << op << " " << generateComplexExpression(depth - 1);

    return expression.str();
}

TEST(ParserTests, ConstantExpressionParsing) {
    const std::string filename = "expressions.v";
    srand(static_cast<unsigned int>(time(nullptr)));
    std::cout << std::filesystem::current_path() << std::endl;
    std::ofstream outFile(filename);
    outFile << "42 - 80 - 79 / 33 - 62 / 40 + 42 - 52" << std::endl;
    outFile.close();

    auto parser = Parser(filename);
    auto *ast = parser.parseConstantExpr();
    std::cout << ast->toString() << std::endl;

    std::remove(filename.c_str());
}
