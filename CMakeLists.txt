project(parsing-techniques)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")

include_directories(include/)
include_directories(src/)

file(GLOB Sources ./src/*.cc ./src/*.cpp)

add_executable(main ${Sources})

add_executable(test_dy
        src/clike_grammar.cc
        src/clike_parser.cc
        src/finite_automaton.cc
        src/regex_parser.cc
        src/tokenizer.cc
        src/clike_ast.cc
        src/interpreter.cc
        src/variable_table.cc
        test/test_dy.cc)

add_executable(test_mem_manager
        test/test_mem_manager.cc)

add_executable(test_regex_parser
        test/test_regex_parser.cc
        src/finite_automaton.cc
        src/regex_parser.cc)

add_executable(test_tokenizer
        test/test_tokenizer.cc
        src/finite_automaton.cc
        src/regex_parser.cc
        src/tokenizer.cc)

add_executable(test_clike_parser
        src/clike_grammar.cc
        src/clike_parser.cc
        src/finite_automaton.cc
        src/regex_parser.cc
        src/tokenizer.cc
        src/clike_ast.cc
        test/test_clike_parser.cc)

