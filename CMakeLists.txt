cmake_minimum_required(VERSION 3.22)
project(veripython)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
        Lexer.h
        AST.h
        AST.cpp
        RtlModule.cpp
        RtlModule.h
        CombLogics.cpp
        CombLogics.h
        CircuitSymbol.cpp
        CircuitSymbol.h
        RtlVisualizer.cpp
        RtlVisualizer.h
        RtlSimulator.cpp
        RtlSimulator.h
)

target_link_libraries(veripython lexer -lfl)

enable_testing()
add_executable(ParserTests
        tests/ParserTests.cpp
        Parser.cpp
        Parser.h
        Lexer.h
        AST.h
        AST.cpp
        RtlModule.cpp
        RtlModule.h
        CircuitSymbol.cpp
        CircuitSymbol.h
        CombLogics.cpp
        CombLogics.h
        RtlVisualizer.cpp
        RtlVisualizer.h
        RtlSimulator.cpp
        RtlSimulator.h
)
target_link_libraries(ParserTests GTest::gtest_main)
target_link_libraries(ParserTests lexer -lfl)
include(GoogleTest)
gtest_discover_tests(ParserTests)
