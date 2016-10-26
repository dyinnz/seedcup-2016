//
// Created by coder on 16-9-8.
//

#define CATCH_CONFIG_MAIN
#define DEBUG

#include <fstream>

#include "catch.hpp"

#include "tokenizer.h"
#include "simplelogger.h"

#include "clike_grammar.h"

using namespace simple_logger;
using namespace clike_grammar;

using std::string;
using std::vector;
using std::ifstream;

BaseLogger logger;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static std::streampos GetFileLength(ifstream &fin) {
  auto backup = fin.tellg();
  fin.seekg(0, fin.end);
  auto length = fin.tellg();
  fin.seekg(backup, fin.beg);
  return length;
}

char *ReadFileData(const string &path, size_t &size) {
  ifstream fin(path);
  if (!fin) {
    return nullptr;
  }

  size = static_cast<size_t>(GetFileLength(fin));
  auto data = new char[size + 1];
  memset(data, 0, sizeof(char) * (size + 1));

  if (fin.read(data, size)) {
    return data;

  } else {
    delete data;
    return nullptr;
  }
}


#define GET_FILE_DATA_SAFELY(name, size, relative_path) \
size_t size = 0; \
const char *name = ReadFileData((relative_path), (size)); \
ScopeGuard [&] { delete[] name; };

TEST_CASE("sample4.txt") {
  GET_FILE_DATA_SAFELY(data, size, "test/data/sample4.txt");

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);

  std::cout << data << std::endl;

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }

  REQUIRE(result);

  REQUIRE(15 == tokens.size());

  REQUIRE(tokens[0].symbol == kInt);
  REQUIRE(tokens[1].symbol == kIdentifier);
  REQUIRE(tokens[2].symbol == kComma);
  REQUIRE(tokens[3].symbol == kIdentifier);
  REQUIRE(tokens[4].symbol == kComma);
  REQUIRE(tokens[5].symbol == kIdentifier);
  REQUIRE(tokens[6].symbol == kAssign);
  REQUIRE(tokens[7].symbol == kNumber);
  REQUIRE(tokens[8].symbol == kSemicolon);
  REQUIRE(tokens[9].symbol == kIdentifier);
  REQUIRE(tokens[10].symbol == kAssign);
  REQUIRE(tokens[11].symbol == kIdentifier);
  REQUIRE(tokens[12].symbol == kAssign);
  REQUIRE(tokens[13].symbol == kIdentifier);
  REQUIRE(tokens[14].symbol == kSemicolon);

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }
}

TEST_CASE("sample6.txt") {
  GET_FILE_DATA_SAFELY(data, size, "test/data/sample6.txt");

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);

  REQUIRE(result);

  REQUIRE(27 == tokens.size());

  REQUIRE(tokens[0].symbol == kInt);
  REQUIRE(tokens[1].symbol == kIdentifier);
  REQUIRE(tokens[2].symbol == kAssign);
  REQUIRE(tokens[3].symbol == kNumber);
  REQUIRE(tokens[4].symbol == kComma);
  REQUIRE(tokens[5].symbol == kIdentifier);
  REQUIRE(tokens[6].symbol == kAssign);
  REQUIRE(tokens[7].symbol == kNumber);
  REQUIRE(tokens[8].symbol == kSemicolon);
  REQUIRE(tokens[9].symbol == kInt);
  REQUIRE(tokens[10].symbol == kIdentifier);
  REQUIRE(tokens[11].symbol == kAssign);
  REQUIRE(tokens[12].symbol == kIdentifier);
  REQUIRE(tokens[13].symbol == kEQ);
  REQUIRE(tokens[14].symbol == kIdentifier);
  REQUIRE(tokens[15].symbol == kSemicolon);

  REQUIRE(tokens[16].symbol == kPrintf);
  REQUIRE(tokens[17].symbol == kLeftParen);
  REQUIRE(tokens[18].symbol == kString);
  REQUIRE(tokens[19].symbol == kComma);
  REQUIRE(tokens[20].symbol == kInc);
  REQUIRE(tokens[21].symbol == kIdentifier);
  REQUIRE(tokens[22].symbol == kAdd);
  REQUIRE(tokens[23].symbol == kIdentifier);
  REQUIRE(tokens[24].symbol == kDec);
  REQUIRE(tokens[25].symbol == kRightParen);
  REQUIRE(tokens[26].symbol == kSemicolon);

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }
}

TEST_CASE("sample7.txt") {
  GET_FILE_DATA_SAFELY(data, size, "test/data/sample7.txt");

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);

  REQUIRE(result);

  REQUIRE(30 == tokens.size());

  REQUIRE(tokens[0].symbol == kInt);
  REQUIRE(tokens[1].symbol == kIdentifier);
  REQUIRE(tokens[2].symbol == kAssign);
  REQUIRE(tokens[3].symbol == kNumber);
  REQUIRE(tokens[4].symbol == kComma);
  REQUIRE(tokens[5].symbol == kIdentifier);
  REQUIRE(tokens[6].symbol == kAssign);
  REQUIRE(tokens[7].symbol == kNumber);
  REQUIRE(tokens[8].symbol == kComma);
  REQUIRE(tokens[9].symbol == kIdentifier);
  REQUIRE(tokens[10].symbol == kAssign);
  REQUIRE(tokens[11].symbol == kNumber);
  REQUIRE(tokens[12].symbol == kSemicolon);

  REQUIRE(tokens[13].symbol == kIdentifier);
  REQUIRE(tokens[14].symbol == kAssign);
  REQUIRE(tokens[15].symbol == kIdentifier);
  REQUIRE(tokens[16].symbol == kInc);
  REQUIRE(tokens[17].symbol == kAdd);
  REQUIRE(tokens[18].symbol == kInc);
  REQUIRE(tokens[19].symbol == kIdentifier);
  REQUIRE(tokens[20].symbol == kMul);
  REQUIRE(tokens[21].symbol == kNumber);
  REQUIRE(tokens[22].symbol == kSemicolon);

  REQUIRE(tokens[23].symbol == kIdentifier);
  REQUIRE(tokens[24].symbol == kAssign);
  REQUIRE(tokens[25].symbol == kIdentifier);
  REQUIRE(tokens[26].symbol == kAdd);
  REQUIRE(tokens[27].symbol == kDec);
  REQUIRE(tokens[28].symbol == kIdentifier);
  REQUIRE(tokens[29].symbol == kSemicolon);

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }
}

TEST_CASE("sample8.txt") {
  GET_FILE_DATA_SAFELY(data, size, "test/data/sample8.txt");

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);

  REQUIRE(result);

  REQUIRE(15 == tokens.size());

  REQUIRE(tokens[0].symbol == kInt);
  REQUIRE(tokens[1].symbol == kIdentifier);
  REQUIRE(tokens[2].symbol == kSemicolon);
  REQUIRE(tokens[3].symbol == kInt);
  REQUIRE(tokens[4].symbol == kIdentifier);
  REQUIRE(tokens[5].symbol == kSemicolon);
  REQUIRE(tokens[6].symbol == kIdentifier);
  REQUIRE(tokens[7].symbol == kAssign);
  REQUIRE(tokens[8].symbol == kNumber);
  REQUIRE(tokens[9].symbol == kSemicolon);
  REQUIRE(tokens[10].symbol == kSemicolon);
  REQUIRE(tokens[11].symbol == kIdentifier);
  REQUIRE(tokens[12].symbol == kAssign);
  REQUIRE(tokens[13].symbol == kNumber);
  REQUIRE(tokens[14].symbol == kSemicolon);

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }
}

TEST_CASE("loop_1.txt") {
  GET_FILE_DATA_SAFELY(data, size, "test/data/loop_1.txt");

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);

  REQUIRE(result);

  REQUIRE(16 == tokens.size());

  REQUIRE(tokens[0].symbol == kInt);
  REQUIRE(tokens[1].symbol == kIdentifier);
  REQUIRE(tokens[2].symbol == kSemicolon);
  REQUIRE(tokens[3].symbol == kFor);
  REQUIRE(tokens[4].symbol == kLeftParen);
  REQUIRE(tokens[5].symbol == kSemicolon);
  REQUIRE(tokens[6].symbol == kSemicolon);
  REQUIRE(tokens[7].symbol == kRightParen);
  REQUIRE(tokens[8].symbol == kLeftBrace);
  REQUIRE(tokens[9].symbol == kBreak);
  REQUIRE(tokens[10].symbol == kSemicolon);
  REQUIRE(tokens[11].symbol == kIdentifier);
  REQUIRE(tokens[12].symbol == kAssign);
  REQUIRE(tokens[13].symbol == kNumber);
  REQUIRE(tokens[14].symbol == kSemicolon);
  REQUIRE(tokens[15].symbol == kRightBrace);

  for (auto &t : tokens) {
    std::cout << to_string(t) << std::endl;
  }
}
