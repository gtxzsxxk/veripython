cmake_minimum_required(VERSION 3.22)
project(veripython)

set(CMAKE_CXX_STANDARD 20)

option(RELEASE_CODE_MODE "Turn off google tests and use -O2" OFF)

if (RELEASE_CODE_MODE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -Wno-error=missing-requires")
    add_compile_options("-fsanitize=address")
    link_libraries("-fsanitize=address")

    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
            GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif ()

find_package(FLEX REQUIRED)
set(FLEX_SRC "${CMAKE_SOURCE_DIR}/verilog.l")
set(FLEX_OUT "${CMAKE_BINARY_DIR}/lex.yy.c")
set(FLEX_DIR "${CMAKE_BINARY_DIR}")
add_custom_command(
        OUTPUT ${FLEX_OUT}
        COMMAND cp ${CMAKE_SOURCE_DIR}/Lexer.h ${FLEX_DIR}
        COMMAND ${FLEX_EXECUTABLE} ${FLEX_SRC}
        MAIN_DEPENDENCY ${FLEX_SRC} ${CMAKE_SOURCE_DIR}/Lexer.h
        COMMENT "Generating lex.yy.c from verilog.l"
)
add_library(lexer OBJECT ${FLEX_OUT})

set(FIRTOOL_LIB_PATH "/root/circt/firtool-1.93.1/lib")
set(FIRTOOL_INSTALL_PATH "${FIRTOOL_LIB_PATH}/cmake")
find_package(CIRCT REQUIRED CONFIG
             HINTS "${FIRTOOL_INSTALL_PATH}/circt")
find_package(MLIR REQUIRED CONFIG
             HINTS "${FIRTOOL_INSTALL_PATH}/mlir")
find_package(LLVM REQUIRED CONFIG
             HINTS "${FIRTOOL_INSTALL_PATH}/llvm")
message(STATUS "Found CIRCT include dir ${CIRCT_INCLUDE_DIRS}")

include_directories(${CIRCT_INCLUDE_DIRS})
link_directories(${FIRTOOL_LIB_PATH})

set(LLVM_ENABLE_RTTI ON)
set(LLVM_ENABLE_EH ON)
set(BUILD_SHARED_LIBS ON)
add_definitions(${LLVM_DEFINITIONS})

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
        SequentialLogics.cpp
        SequentialLogics.h
        Lexer.cpp
        EmitFIRRTL.cpp
        EmitFIRRTL.h
)

target_link_libraries(veripython PRIVATE lexer -lfl)
target_link_libraries(veripython PRIVATE
        CIRCTFIRRTL
        CIRCTFirtool
        MLIRIR
)

if (NOT RELEASE_CODE_MODE)
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
            SequentialLogics.cpp
            SequentialLogics.h
            EmitFIRRTL.cpp
            EmitFIRRTL.h
    )
    target_link_libraries(ParserTests PRIVATE GTest::gtest_main)
    target_link_libraries(ParserTests PRIVATE lexer -lfl)
    target_link_libraries(ParserTests PRIVATE
            CIRCTFIRRTL
            CIRCTFirtool
            MLIRIR
    )
    add_executable(AcrilatorHeaderCppTests tests/ArcilatorHeaderCppTests.cpp)
    target_link_libraries(AcrilatorHeaderCppTests PRIVATE GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(ParserTests)
endif ()
