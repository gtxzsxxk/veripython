//
// Created by unknown on 2024/12/14.
//

#ifndef VERIPYTHON_EXCEPTIONS_H
#define VERIPYTHON_EXCEPTIONS_H

#include <stdexcept>

class ParsingException : public std::runtime_error {
public:
    explicit
    ParsingException(const std::string &problem) : std::runtime_error(problem) {}
};

class CircuitException : public std::runtime_error {
public:
    explicit
    CircuitException(const std::string &problem) : std::runtime_error(problem) {}
};

#endif //VERIPYTHON_EXCEPTIONS_H
