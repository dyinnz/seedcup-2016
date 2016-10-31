project(seedcup2016)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")

include_directories(common/)
include_directories(src/)

file(GLOB Sources src/*.cc src/*.h common/*.h)

add_executable(SeedCup2016.exe ${Sources})




###############################################################################

add_executable(test_dy
        src/clike_grammar.cc
        src/clike_parser.cc
        src/finite_automaton.cc
        src/regex_parser.cc
        src/tokenizer.cc
        src/ast.cc
        src/clike_interpreter.cc
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
        src/ast.cc
        test/test_clike_parser.cc)

