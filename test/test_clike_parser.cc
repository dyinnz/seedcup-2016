//
// Created by Dyinnz on 16-10-25.
//

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "simplelogger.h"
#include "test_utility.h"
#include "clike_grammar.h"
#include "clike_parser.h"

using namespace simple_logger;
BaseLogger logger;

using std::string;
using std::vector;

vector<Token> GetTokensFromFile(const string &filename) {
  GET_FILE_DATA_SAFELY(data, size, filename)
  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data + size, tokens);
  REQUIRE(result);

  for (auto &t : tokens) {
    logger.debug("{}", to_string(t));
  }

  return tokens;
}

TEST_CASE("Empty") {
  logger.set_log_level(kDebug);

  auto tokens = GetTokensFromFile("test/data/dy-test-1.c");
  REQUIRE(0 == tokens.size());

  ClikeParser parser;
  parser.Parse(tokens);
}

TEST_CASE("Comments") {
  auto tokens = GetTokensFromFile("test/data/dy-test-2.c");
  REQUIRE(0 == tokens.size());

  ClikeParser parser;
  parser.Parse(tokens);
}

TEST_CASE("Declaration & Definition") {
  auto tokens = GetTokensFromFile("test/data/dy-test-3.c");

  ClikeParser parser;
  parser.Parse(tokens);
}

TEST_CASE("if & else") {
  auto tokens = GetTokensFromFile("test/data/dy-test-4.c");

  ClikeParser parser;
  parser.Parse(tokens);
}