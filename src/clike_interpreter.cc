//
// Created by Dyinnz on 16-10-25.
//

#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <unordered_set>
#include "clike_interpreter.h"
#include "clike_grammar.h"
#include "simplelogger.h"

using namespace simple_logger;
using namespace clike_grammar;
using namespace std;

/**
 * @brief  Start interpret
 */
void ClikeInterpreter::Exec() {
  is_break_ = false;
  last_line_ = 0;
  ExecBlock(ast_.root());
}

/**
 * @param node A AST node wants to interpret
 * @return  result of this line of code
 * @brief Interpret a single line
 */
int ClikeInterpreter::ExecSingle(AstNode *node) {
  static std::unordered_set<Symbol> expr_head = {
    kInc, kDec,
    kAdd, kSub, kMul, kDiv, kAssign,
    kEQ, kNE, kGE, kGT, kLE, kLT,
    kNumber, kIdentifier, kComma
  };


  if (expr_head.find(node->symbol()) != expr_head.end()) {
    return EvalExpr(node);
  }

  int result = 0;
  switch (node->symbol().ID()) {
    case kSemicolonID:
      // empty statement
      break;

    case kBlockID:ExecBlock(node);
      break;

    case kIntID:ExecTypeHead(node);
      break;

    case kPrintfID:result = ExecPrintf(node);
      break;

    case kIfRootID:ExecIfRoot(node);
      break;

    case kForID:ExecFor(node);
      break;

    case kWhileID:ExecWhile(node);
      break;

    case kDoID:ExecDoWhile(node);
      break;

//  case kAssignID:result = ExecAssign(node);
//    break;

    case kBreakID:
      is_break_ = true;
      recordLine(node);
      break;

    case kIfID:
    case kElseID:func_error(logger, "illegal node: {}", node->to_string());
      break;

    default:func_error(logger, "unrecognized node: {}", node->to_string());
  }

  return result;
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret a block of code
 */
void ClikeInterpreter::ExecBlock(AstNode *node) {
  table_.PushLevel();
  for (auto child : node->children()) {
    ExecSingle(child);
    if (is_break_) {
      break;
    }
  }
  table_.PopLevel();
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret a block of code of a loop struct
 */
void ClikeInterpreter::ExecLoopBlock(AstNode *node) {
  table_.EnterLevel();
  for (auto child : node->children()) {
    ExecSingle(child);
    if (is_break_) {
      break;
    }
  }
  table_.LeaveLevel();
}

/**
 * @param node A AST node wants to interpret
 * @brief Define a var
 */
void ClikeInterpreter::ExecTypeHead(AstNode *node) {
  for (auto child : node->children()) {
    if (kIdentifier == child->symbol()) {
      table_.NewInt(child->text());
      continue;

    } else if (kAssign == child->symbol()) {
      auto id_node = child->children().front();
      auto expr = child->children().back();
      auto result = EvalExpr(expr);

      table_.NewInt(id_node->text(), result);
      func_debug(logger, "name: {}, value: {}", id_node->text(), result);

    } else {
      func_error(logger, "illegal node: {}", child->to_string());
    }
  }
}

/**
 * @param node A AST node wants to interpret
 * @return  result of this expr
 * @brief Interpret a expr
 */
int ClikeInterpreter::EvalExpr(AstNode *node) {
  int result;
  recordLine(node);
  switch (node->symbol().ID()) {
    case kNumberID:
      func_debug(logger, "Number's string is: {}", node->text());
      result = stoi(node->text(), 0, 0);
      func_debug(logger, "Number is: {}", result);
      return result;

    case kIdentifierID:
      return table_.GetInt(node->text());

    case kCommaID:EvalExpr(node->children().front());
      return EvalExpr(node->children().back());

    case kAssignID: {
      auto id_name = node->children().front()->text();
      auto result = EvalExpr(node->children().back());
      table_.SetInt(id_name, result);
      return result;
    }

    case kEQID:
      return EvalExpr(node->children().front())
          == EvalExpr(node->children().back());
    case kNEID:
      return EvalExpr(node->children().front())
          != EvalExpr(node->children().back());

    case kLEID:
      return EvalExpr(node->children().front())
          <= EvalExpr(node->children().back());

    case kGEID:
      return EvalExpr(node->children().front())
          >= EvalExpr(node->children().back());

    case kLTID:
      return EvalExpr(node->children().front())
          < EvalExpr(node->children().back());

    case kGTID:
      return EvalExpr(node->children().front())
          > EvalExpr(node->children().back());

    case kAddID:
      if (1 == node->children().size()) {
        return EvalExpr(node->children().front());
      } else {
        return EvalExpr(node->children().front())
            + EvalExpr(node->children().back());
      }

    case kSubID:
      if (1 == node->children().size()) {
        return -EvalExpr(node->children().front());
      } else {
        return EvalExpr(node->children().front())
            - EvalExpr(node->children().back());
      }

    case kMulID:
      return EvalExpr(node->children().front())
          * EvalExpr(node->children().back());

    case kDivID:
      return EvalExpr(node->children().front())
          / EvalExpr(node->children().back());

    case kIncID: {
      auto result = EvalExpr(node->children().front());
      auto id_name = node->children().front()->text();
      table_.SetInt(id_name, table_.GetInt(id_name) + 1);
      return result;
    }

    case kDecID: {
      auto result = EvalExpr(node->children().front());
      auto id_name = node->children().front()->text();
      table_.SetInt(id_name, table_.GetInt(id_name) - 1);
      return result;
    }

    case kStringID:
      return 1;   //Always return true

    case kPrintfID:
      return ExecPrintf(node);

    default:func_error(logger, "illegal node: {}", node->to_string());
      return 0;
  }
}

/**
 * @param node A AST node wants to interpret
 * @return  result of printf()
 * @brief Interpret printf()
 */
int ClikeInterpreter::ExecPrintf(AstNode *node) {
  auto raw_text = node->children().front()->text();
  auto child_iter = node->children().begin() + 1;

  std::string text = "";

  for (size_t i = 1; i < raw_text.length() - 1; i++) {
    if (raw_text[i] == '\\') {
      i++;
      text += raw_text[i];
      continue;
    }

    if (raw_text[i] == '%') {
      if (raw_text[i + 1] == '%') {
        i++;
        text += raw_text[i];
        continue;
      } else {
        auto expr_result = EvalExpr(*child_iter);
        child_iter++;

        text += to_string(expr_result);
        i++;
        continue;
      }
    }
    text += raw_text[i];
  }

  auto result = text.length();

  // TODO: I have added next line for
  // that there is no expression in the node of Printf
  recordLine(node);
  for (auto child : node->children()) {
    EvalExpr(child);
  }

  func_debug(logger, "printf return {}, val is {}", result, text);

  return result;
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret [if-else if-else] struct
 */
void ClikeInterpreter::ExecIfRoot(AstNode *node) {
  for (auto clause: node->children()) {

    if (clause->symbol() == kIf) {
      if (ExecIfClause(clause)) {
        break;
      }
    } else if (clause ->symbol() == kElse) {
      ExecElseClause(clause);
      break;
    } else {
      func_error(logger, "illegal node: {}, expect kIf or kElse", node->to_string());
    }

  }
}

/**
 * @param node A AST node wants to interpret
 * @return result of if
 * @brief Interpret [if-else if] struct
 */
int ClikeInterpreter::ExecIfClause(AstNode *node) {
  if (kIf != node->symbol()) {
    return 0;
  }

  int result = 0;
  auto head = node->children().front();
  auto body = node->children().back();

  result = EvalExpr(head);

  if (result) {
    ExecSingle(body);
  }
   return result;
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret [else] struct
 */
void ClikeInterpreter::ExecElseClause(AstNode *node) {
  if (kElse != node->symbol()) {
    return;
  }

  ExecSingle(node->children().front());
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret [for] struct
 */
void ClikeInterpreter::ExecFor(AstNode *node) {
  if (kFor != node->symbol()) {
    return;
  }

  auto init = node->children().at(0);
  auto condition = node->children().at(1);
  auto step = node->children().at(2);
  auto body = node->children().at(3);

  recordLine(node);
  for (ExecSingle(init); ExecSingle(condition) || condition->symbol() == kSemicolon; ExecSingle(step)) {
    if (body->symbol() == kBlock) {
      ExecLoopBlock(body);
    } else {
      ExecSingle(body);
    }

    if (is_break_) {
      is_break_ = false;
      break;
    }
  }

  if (body->symbol() == kBlock) {
    table_.PopToNowLevel();
  }
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret [while] struct
 */
void ClikeInterpreter::ExecWhile(AstNode *node) {
  if (kWhile != node->symbol()) {
    return;
  }

  auto condition = node->children().front();
  auto body = node->children().back();

  while (ExecSingle(condition)) {
    if (body->symbol() == kBlock) {
      ExecLoopBlock(body);
    } else {
      ExecSingle(body);
    }

    if (is_break_) {
      is_break_ = false;
      break;
    }
  }

  if (body->symbol() == kBlock) {
    table_.PopToNowLevel();
  }
}

/**
 * @param node A AST node wants to interpret
 * @brief Interpret [do-while] struct
 */
void ClikeInterpreter::ExecDoWhile(AstNode *node) {
  if (kDo != node->symbol()) {
    return;
  }

  auto body = node->children().front();
  auto condition = node->children().back();

  do {
    if (body->symbol() == kBlock) {
      ExecLoopBlock(body);
    } else {
      ExecSingle(body);
    }

    if (is_break_) {
      is_break_ = false;
      break;
    }
  } while (ExecSingle(condition));

  if (body->symbol() == kBlock) {
    table_.PopToNowLevel();
  }
}

/**
 * @param filename output file
 * @brief output line number information
 */
void ClikeInterpreter::OutputLines(const char *filename) {
  std::ofstream fout(filename);
  for (size_t i = 0; i < run_lines_.size(); ++i) {
    if (i != run_lines_.size() - 1)
      fout << run_lines_[i] << " ";
    else
      fout << run_lines_[i];
  }
  fout.close();
}

/**
 * @param node A AST node interpreting
 * @brief record line infomation
 */
void ClikeInterpreter::recordLine(AstNode *node) {
  if (last_line_ != node->row()) {
    func_log(logger, "now running line {}: node {}", node->row(), node->to_string());
    run_lines_.push_back(node->row());
    last_line_ = node->row();
  }
  table_.Print();
}
