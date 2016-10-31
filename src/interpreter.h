//
// Created by coder on 16-10-25.
//

#pragma once

#include <vector>
#include "ast.h"
#include "variable_table.h"

class Interpreter {
 public:
  Interpreter(Ast &&ast) : ast_(std::move(ast)) {}

  void Exec();
  int ExecSingle(AstNode *node);
  void ExecBlock(AstNode *node);
  void ExecLoopBlock(AstNode *node);

  void ExecTypeHead(AstNode *node);
  int ExecPrintf(AstNode *node);

  void ExecIfRoot(AstNode *node);
  int ExecIfClause(AstNode *node);
  void ExecElseClause(AstNode *node);

  void ExecFor(AstNode *node);
  void ExecWhile(AstNode *node);
  void ExecDoWhile(AstNode *node);

  int EvalExpr(AstNode *node);

  void OutputLines(const char *filename);

 private:

  Ast ast_;
  symbol_table::SymbolTable table_;
  std::vector<size_t> run_lines_;

  bool is_break_;
  std::size_t last_line_;

  void recordLine(AstNode *node);
};
