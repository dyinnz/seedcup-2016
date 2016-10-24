//
// Created by coder on 16-10-24.
//

#include <iostream>
#include <fstream>
#include <cstring>

#include "simplelogger.h"
#include "clike_grammar.h"
#include "clike_parser.h"

using namespace std;
using namespace simple_logger;

BaseLogger logger;

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

int main() {
  logger.set_log_level(kDebug);
  GET_FILE_DATA_SAFELY(data, size, "test/data/dy-debug");
  logger.debug("\n{}", data);

  auto tokenizer = clike_grammar::BuilderClikeTokenizer();
  vector<Token> tokens;
  auto result = tokenizer.LexicalAnalyze(data, data+size, tokens);
  if (!result) {
    return -1;
  }

  for (auto &t : tokens) {
    logger.debug("{}", to_string(t));
  }

  ClikeParser parser;
  parser.Parse(tokens);

  return 0;
}
