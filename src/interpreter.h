//
// Created by coder on 16-10-25.
//

#pragma once

#include <vector>
#include "clike_ast.h"
#include "symbol_table.h"

class Interpreter {
 public:
  Interpreter(Ast &&ast) : ast_(std::move(ast)) {}

  void Exec();
  void ExecSingle(AstNode *node);
  void ExecBlock(AstNode *node);

  void ExecTypeHead(AstNode *node);
  void ExecPrintf(AstNode *node);

  void ExecIfRoot(AstNode *node);
  void ExecIfClause(AstNode *node);
  void ExecElseClause(AstNode *node);

  void ExecFor(AstNode *node);
  void ExecWhile(AstNode *node);
  void ExecDoWhile(AstNode *node);

  void ExecAssign(AstNode *node);
  int EvalExpr(AstNode *node);

 private:
  Ast ast_;
  symbol_table::SymbolTable table_;
  std::vector<size_t> run_lines_;
};
