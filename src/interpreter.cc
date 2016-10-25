//
// Created by coder on 16-10-25.
//

#include <cstdlib>
#include "interpreter.h"
#include "clike_grammar.h"
#include "simplelogger.h"

using namespace simple_logger;
using namespace clike_grammar;

void Interpreter::Exec() {
  ExecBlock(ast_.root());
}

void Interpreter::ExecSingle(AstNode *node) {
  switch (node->symbol().ID()) {
    case kSemicolonID:
      // empty statement
      break;

    case kBlockID:ExecBlock(node);
      break;

    case kIntID:ExecTypeHead(node);
      break;

    case kPrintfID:ExecPrintf(node);
      break;

    case kIfRootID:ExecIfRoot(node);
      break;

    case kForID:ExecFor(node);
      break;

    case kWhileID:ExecWhile(node);
      break;

    case kDoID:ExecDoWhile(node);
      break;

    case kAssignID:ExecAssign(node);
      break;

    case kIfID:
    case kElseID:
    case kBreakID:func_error(logger, "illegal node: {}", node->to_string());
      break;

    default:func_error(logger, "unrecognized node: {}", node->to_string());
  }
}

void Interpreter::ExecBlock(AstNode *node) {
  table_.PushLevel();
  for (auto child : node->children()) {
    ExecSingle(child);
  }
  table_.PopLevel();
}

void Interpreter::ExecTypeHead(AstNode *node) {
  bool count_line = false;
  for (auto child : node->children()) {
    if (kIdentifier == child->symbol()) {
      // ignore it
      continue;

    } else if (kAssign == child->symbol()) {
      auto id_node = child->children().front();
      auto expr = child->children().back();
      auto result = EvalExpr(expr);

      table_.SetInt(id_node->str(), result);
      func_debug(logger, "name: {}, value: {}", id_node->str(), result);

    } else {
      func_error(logger, "illegal node: {}", child->to_string());
    }
  }
}

void Interpreter::ExecAssign(AstNode *node) {
  // TODO: need recording line no
  EvalExpr(node);
  auto name = node->children().front()->str();
  func_debug(logger, "name: {}, value: {}", name, table_.GetInt(name));
}

int Interpreter::EvalExpr(AstNode *node) {
  switch (node->symbol().ID()) {
    case kNumberID:
      return atoi(node->str().c_str());

    case kIdentifierID:
      return table_.GetInt(node->str());

    case kCommaID:EvalExpr(node->children().front());
      return EvalExpr(node->children().back());

    case kAssignID: {
      auto id_name = node->children().front()->str();
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
      auto id_name = node->children().front()->str();
      table_.SetInt(id_name, table_.GetInt(id_name) + 1);
      return result;
    }

    case kDecID: {
      auto result = EvalExpr(node->children().front());
      auto id_name = node->children().front()->str();
      table_.SetInt(id_name, table_.GetInt(id_name) - 1);
      return result;
    }

    default:func_error(logger, "illegal node: {}", node->to_string());
      return 0;
  }
}

void Interpreter::ExecPrintf(AstNode *node) {
  for (auto child : node->children()) {
    EvalExpr(child);
  }
}

void Interpreter::ExecIfRoot(AstNode *node) {

}

void Interpreter::ExecFor(AstNode *node) {

}

void Interpreter::ExecWhile(AstNode *node) {

}

void Interpreter::ExecDoWhile(AstNode *node) {

}
