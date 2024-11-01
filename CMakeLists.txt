cmake_minimum_required(VERSION 3.22)
project(veripython)

set(CMAKE_CXX_STANDARD 20)

find_package(FLEX REQUIRED)

set(FLEX_SRC "${CMAKE_SOURCE_DIR}/verilog.l")
set(FLEX_OUT "${CMAKE_BINARY_DIR}/lex.yy.c")
set(FLEX_DIR "${CMAKE_BINARY_DIR}")

add_custom_command(
        OUTPUT  ${FLEX_OUT}
        COMMAND cp ${CMAKE_SOURCE_DIR}/Lexer.h ${FLEX_DIR}
        COMMAND ${FLEX_EXECUTABLE}  ${FLEX_SRC}
        MAIN_DEPENDENCY ${FLEX_SRC} ${CMAKE_SOURCE_DIR}/Lexer.h
        COMMENT "Generating lex.yy.c from verilog.l"
)

add_library(lexer OBJECT ${FLEX_OUT})

add_executable(veripython
        main.cpp
        Parser.cpp
        Parser.h
        Lexer.h)

target_link_libraries(veripython lexer -lfl)
