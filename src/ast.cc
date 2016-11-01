//
// Created by Dyinnz on 16-10-24.
//

#include "ast.h"
#include "simplelogger.h"

using namespace simple_logger;

/**
 * @see ast.h
 */
void PrintAstNode(AstNode *node, int deep) {
  for (int i = 0; i < deep; ++i) {
    std::cout << "    ";
  }

  std::cout << node->to_string() << std::endl;

  for (auto child : node->children()) {
    PrintAstNode(child, deep + 1);
  }
}
