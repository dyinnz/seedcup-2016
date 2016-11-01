project(seedcup2016)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")

include_directories(common/)
include_directories(src/)

file(GLOB Sources src/*.cc src/*.h common/*.h)

add_executable(SeedCup2016.exe ${Sources})




###############################################################################

# Add some object library

add_library(regex OBJECT
  src/finite_automaton.cc
  src/regex_parser.cc)

add_library(tokenizer OBJECT
  src/tokenizer.cc)

add_library(ast OBJECT
  src/ast.cc)

add_library(clike_grammar OBJECT
  src/clike_grammar.cc)

add_library(clike_parser OBJECT
  src/clike_parser.cc)

add_library(clike_interpreter OBJECT
  src/clike_interpreter.cc
  src/variable_table.cc)

# Add test executable

add_executable(test_mem_manager
  test/test_mem_manager.cc)

add_executable(test_regex_parser
  $<TARGET_OBJECTS:regex>
  test/test_regex_parser.cc)

add_executable(test_tokenizer
  $<TARGET_OBJECTS:regex>
  $<TARGET_OBJECTS:tokenizer>
  test/test_tokenizer.cc)

add_executable(test_clike_parser
  $<TARGET_OBJECTS:regex>
  $<TARGET_OBJECTS:tokenizer>
  $<TARGET_OBJECTS:ast>
  $<TARGET_OBJECTS:clike_grammar>
  $<TARGET_OBJECTS:clike_parser>
  test/test_clike_parser.cc)


add_executable(test_dy
  $<TARGET_OBJECTS:regex>
  $<TARGET_OBJECTS:tokenizer>
  $<TARGET_OBJECTS:ast>
  $<TARGET_OBJECTS:clike_grammar>
  $<TARGET_OBJECTS:clike_parser>
  $<TARGET_OBJECTS:clike_interpreter>
  test/test_dy.cc)
