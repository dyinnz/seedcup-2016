//
// Created by coder on 16-9-8.
//

#define CATCH_CONFIG_MAIN
#define DEBUG

#include "catch.hpp"

#include "tokenizer.h"
#include "simplelogger.h"

using namespace simple_logger;

using std::string;
using std::vector;

BaseLogger logger;

#define DEF_TEST_TERMINAL(name, offset) \
static const Symbol name(Symbol::kTerminal, kStartID + (offset));

/*----------------------------------------------------------------------------*/

DEF_TEST_TERMINAL(k110, 1);
DEF_TEST_TERMINAL(kNumber, 2);
DEF_TEST_TERMINAL(kIf, 3);
DEF_TEST_TERMINAL(kWord, 4);

TEST_CASE("Build DFA") {
  TokenizerBuilder tokenizer_builder;
  tokenizer_builder.SetPatterns(
      {{"110", k110},
       {"[0123]+", kNumber}});
  auto tokenizer = tokenizer_builder.Build();
  auto dfa = tokenizer.GetTokenDFA();

  REQUIRE(dfa->Match("110"));
  REQUIRE(dfa->Match("01230123"));
  REQUIRE_FALSE(dfa->Match("1104"));
}

TEST_CASE("Lexical analyse") {
  TokenizerBuilder tokenizer_builder;
  tokenizer_builder.SetPatterns({{"if", kIf},
                                 {R"(\d+)", kNumber},
                                 {R"(\w+)", kWord},
                                 {"[ \t\v\f\r]", kSpaceSymbol},
                                 {"\n", kLFSymbol}
                                });
  auto tokenizer = tokenizer_builder.Build();

  string s{"if there\tare\n\n1000 dogs"};

  vector<Token> tokens;
  bool result = tokenizer.LexicalAnalyze(s, tokens);
  REQUIRE(result);

  REQUIRE(6 == tokens.size());

  REQUIRE(tokens[0].str == "if");
  REQUIRE(tokens[1].str == "there");
  REQUIRE(tokens[2].str == "are");
  REQUIRE(tokens[3].str == "\\n");
  REQUIRE(tokens[4].str == "1000");
  REQUIRE(tokens[5].str == "dogs");

  REQUIRE(tokens[0].symbol == kIf);
  REQUIRE(tokens[1].symbol == kWord);
  REQUIRE(tokens[2].symbol == kWord);
  REQUIRE(tokens[3].symbol == kLFSymbol);
  REQUIRE(tokens[4].symbol == kNumber);
  REQUIRE(tokens[5].symbol == kWord);

  REQUIRE(tokens[0].row == 0);
  REQUIRE(tokens[1].row == 0);
  REQUIRE(tokens[2].row == 0);
  REQUIRE(tokens[3].row == 0);
  REQUIRE(tokens[4].row == 2);
  REQUIRE(tokens[5].row == 2);

  REQUIRE(tokens[0].column == 0);
  REQUIRE(tokens[1].column == 3);
  REQUIRE(tokens[2].column == 9);
  REQUIRE(tokens[3].column == 12);
  REQUIRE(tokens[4].column == 0);
  REQUIRE(tokens[5].column == 5);
}

TEST_CASE("Skip comments") {
  logger.set_log_level(kDebug);

  TokenizerBuilder tokenizer_builder;
  tokenizer_builder.SetPatterns({{R"(\w+)", kNumber},
                                 {"[ \t\v\f\r]", kSpaceSymbol},
                                 {"\n", kLFSymbol},
                                });
  tokenizer_builder.SetLineComment("//");
  tokenizer_builder.SetBlockComment("/*", "*/");

  string s(
      "hello // world\n"
          "/* test block 1 \n"
          " test block 2 */\n"
          "computer world"
  );

  auto tokenizer = tokenizer_builder.Build();
  vector<Token> tokens;
  bool result = tokenizer.LexicalAnalyze(s, tokens);
  for (auto &token : tokens) {
    logger.debug("{}", to_string(token));
  }
}
