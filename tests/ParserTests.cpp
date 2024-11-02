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

TEST(ParserTests, SimpleConstantExpressionParsing) {
    const std::string filename = "expressions.v";
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < 100; i++) {
        std::ofstream outFile(filename);
        auto expressionString = generateComplexExpressionNoParen((rand() % 6) + 1);
        std::cout << "Testing: " << expressionString << std::endl;
        int realValue = evaluateExpression(expressionString);
        outFile << expressionString << std::endl;
        outFile.close();

        auto parser = Parser(filename);
        auto *ast = parser.parseConstantExpr();
        int parserValue = ast->eval();
        EXPECT_EQ(parserValue, realValue);
        std::remove(filename.c_str());
    }
}

TEST(ParserTests, HaveParenConstantExpressionParsing) {
    const std::string filename = "expressions.v";
    srand(static_cast<unsigned int>(time(nullptr)));

    for (int i = 0; i < 100; i++) {
        std::ofstream outFile(filename);
        auto expressionString = generateComplexExpressionWithParen((rand() % 4) + 1);
        std::cout << "Testing: " << expressionString << std::endl;
        int realValue = evaluateExpression(expressionString);
        outFile << expressionString << std::endl;
        outFile.close();

        auto parser = Parser(filename);
        auto *ast = parser.parseConstantExpr();
        int parserValue = ast->eval();
        EXPECT_EQ(parserValue, realValue);
        std::remove(filename.c_str());
    }
}
