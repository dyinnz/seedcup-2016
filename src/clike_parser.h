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

  AstNode *ParseBlockBody(TokenIterator &p); // the block without {}
  AstNode *ParseBraceBlock(TokenIterator &p); // the block with {}
  AstNode *ParseLineOrBlock(TokenIterator &p); // the block after if & for & while
  AstNode *ParseLine(TokenIterator &p); // the block with {}

  AstNode *ParseTypeHead(TokenIterator &p); // int x, y = 1;
  AstNode *ParsePrintf(TokenIterator &p);
  AstNode *ParseExprStmt(TokenIterator &p); // any expr ending with ;

  /**
   * Expression
   */
  AstNode *ParsePrimaryExpr(TokenIterator &p);
  AstNode *ParsePostfixExpr(TokenIterator &p);
  AstNode *ParsePosNegExpr(TokenIterator &p);
  AstNode *ParseMulDivExpr(TokenIterator &p);
  AstNode *ParseAddSubExpr(TokenIterator &p);
  AstNode *ParseCompareExpr(TokenIterator &p);
  AstNode *ParseEquationExpr(TokenIterator &p);
  AstNode *ParseAssignExpr(TokenIterator &p);
  AstNode *ParseCommaExpr(TokenIterator &p);
  AstNode *ParseExpr(TokenIterator &p);

  AstNode *ParseIf(TokenIterator &p);
  AstNode *ParseFor(TokenIterator &p);
  AstNode *ParseWhile(TokenIterator &p);
  AstNode *ParseDoWhile(TokenIterator &p);

  Ast TransferAst() {
    return std::move(ast_);
  }

 private:
  Ast ast_;
  std::vector<Token> tokens_;
  std::stack<AstNode *> parse_stack_;
  TokenIterator curr_;
};
