//
// Created by Dyinnz on 16-10-25.
//

#pragma once

#include <vector>
#include "ast.h"
#include "variable_table.h"

/**
 * @brief   C-like programming langague Interpreter
 *
 * @details A Interpreter. It traverse a AST, and do as the AST descripted.
 *
 *          Its constructor needs a AST's root node.
 *          When Exec() is executed, the interpreter will traverse th tree and record th line number it walked.
 *          When Exec() returned, call OutptLines to output the result.
 */
class ClikeInterpreter {
 public:

  /**
   * @param A root node of the AST.
   * @brief Interpreter Constructor. Need a AST's root and ready to exec
   */
  ClikeInterpreter(Ast &&ast) : ast_(std::move(ast)) {}

  /**
   * @brief  Start interpret
   */
  void Exec();

  /**
   * @param filename output file
   * @brief output line number information
   */
  void OutputLines(const char *filename);

 private:

  /**
   * record the line number information of this node
   */
  void recordLine(AstNode *node);

  /**
   * Exec statement(s)
   */
  int ExecSingle(AstNode *node);
  void ExecBlock(AstNode *node);
  void ExecLoopBlock(AstNode *node);

  /**
   * Interprete specified syntactic structure
   */
  void ExecTypeHead(AstNode *node);
  int ExecPrintf(AstNode *node);

  void ExecIfRoot(AstNode *node);
  int ExecIfClause(AstNode *node);
  void ExecElseClause(AstNode *node);

  void ExecFor(AstNode *node);
  void ExecWhile(AstNode *node);
  void ExecDoWhile(AstNode *node);

  /**
   * Calculate a expression
   */
  int EvalExpr(AstNode *node);

private:
  Ast ast_;
  symbol_table::VariableTable table_;
  std::vector<size_t> run_lines_;

  bool is_break_;
  std::size_t last_line_;

};
