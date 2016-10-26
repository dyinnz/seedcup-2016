//
// Created by coder on 16-10-24.
//

#include <iostream>

#include "test_utility.h"

#include "simplelogger.h"
#include "clike_grammar.h"
#include "clike_parser.h"
#include "interpreter.h"

using namespace std;
using namespace simple_logger;

BaseLogger logger;

int main() {
  logger.set_log_level(kDebug);
  GET_FILE_DATA_SAFELY(data, size, "test/data/sample3.txt");
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

  Interpreter interpreter(parser.TransferAst());
  interpreter.Exec();

  interpreter.OutputLines("output.txt");

  return 0;
}
