//
// Created by coder on 16-10-24.
//

#pragma once

#include <stack>
#include "clike_ast.h"

class ClikeParser {
  typedef std::vector<Token>::iterator TokenIterator;

 public:
  Ast Parse(std::vector<Token> &tokens);

  AstNode* ParseBlockBody(TokenIterator &p); // the block without {}
  AstNode* ParseBraceBlock(TokenIterator &p); // the block with {}
  AstNode* ParseLine(TokenIterator &p); // the block with {}
  AstNode* ParseSubBlock(TokenIterator &p); // the block after if & for & while

  AstNode *ParseTypeHead(TokenIterator &p); // int x, y = 1;
  AstNode* ParseAssign(TokenIterator &p); // x = 1;

  AstNode* ParseExpr(TokenIterator &p);
  AstNode *ParseUnaryExpr(TokenIterator &p);
  AstNode *ParsePrimaryExpr(TokenIterator &p);
  AstNode *ParseTermExpr(TokenIterator &p);

  AstNode* ParsePrintf(TokenIterator &p);
  AstNode* ParseIf(TokenIterator &p);
  AstNode* ParseFor(TokenIterator &p);
  AstNode* ParseWhile(TokenIterator &p);
  AstNode* ParseDoWhile(TokenIterator &p);

 private:
  Ast ast_;
  std::vector<Token> tokens_;
  std::stack<AstNode *> parse_stack_;
  TokenIterator curr_;
};
