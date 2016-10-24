//
// Created by coder on 16-10-24.
//

#include <iterator>
#include "simplelogger.h"
#include "clike_grammar.h"
#include "clike_parser.h"

using namespace clike_grammar;
using namespace simple_logger;

using std::move;

Ast ClikeParser::Parse(std::vector<Token> &tokens) {
  tokens.push_back(kEofToken);
  curr_ = tokens.begin();

  auto block = ast_.CreateBlock(kBlock);

  auto p = curr_;
  while (true) {
    while (kLFSymbol == p->symbol) {
      ++p;
    }
    if (kEofToken == *p) {
      break;
    }
    logger.debug("new line parse: {}", to_string(*p));

    AstNode *next = nullptr;
    if (kInt == p->symbol) {
      next = ParseTypeHead(p);
    } else if (kIdentifier == p->symbol) {
      next = ParseExprStmt(p);
    } else if (kPrintf == p->symbol) {
      next = ParsePrintf(p);
    } else {
      logger.error("unrecognized token {}", to_string(*p));
      ++p;
    }

    block->push_child_back(next);
  }

  ast_.set_root(block);
  PrintAstNode(ast_.root());

  return move(ast_);
}

AstNode *ClikeParser::ParseTypeHead(TokenIterator &p) {
  // type int
  if (kInt != p->symbol) {
    logger.error("expect a type {}", to_string(*p));
    return nullptr;
  }
  auto decl_node = ast_.CreateTypeHead(move(*p));
  ++p;

  auto parse_decl_def = [&]() -> AstNode * {
    if (kIdentifier != p->symbol) {
      logger.error("expect a identifier {}", to_string(*p));
    }
    auto identifier = ast_.CreateTerminal(move(*p));
    ++p;

    if (kAssign != p->symbol) {
      // There is a declaration
      return identifier;

    } else {
      // There is a definition
      auto assign = ast_.CreateAssign(move(*p));
      ++p;

      // TODO: should use ParseCommaExpr()
      auto expr = ParseAssignExpr(p);

      assign->push_child_back(identifier);
      assign->push_child_back(expr);
      return assign;
    }
  };

  // first declaration or definition
  auto sub_node = parse_decl_def();
  if (!sub_node) {
    logger.log("expect a declaration or definition {}", to_string(*p));
    return nullptr;
  }
  decl_node->push_child_back(sub_node);

  while (kSemicolon != p->symbol) {
    // skip ,
    if (kComma != p->symbol) {
      logger.log("expect a , {}", to_string(*p));
      continue;
    } else {
      ++p;
    }

    if (nullptr == (sub_node = parse_decl_def())) {
      logger.log("expect a declaration or definition {}",
                 to_string(*p));
      return nullptr;
    }
    decl_node->push_child_back(sub_node);
  }

  // skip ;
  ++p;
  return decl_node;
}

AstNode *ClikeParser::ParseExprStmt(TokenIterator &p) {
  auto expr = ParseExpr(p);
  if (kSemicolon != p->symbol) {
    logger.error("expect a ; {}", to_string(*p));
    return nullptr;
  }
  ++p;
  return expr;
}

AstNode *ClikeParser::ParsePrimaryExpr(TokenIterator &p) {
  if (kLeftParen == p->symbol) {
    // begin with (
    ++p;

    auto expr = ParseExpr(p);

    // end with )
    if (kRightParen != p->symbol) {
      logger.error("expect a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    return expr;

  } else {
    // begin with identifier & positive number
    if (kNumber != p->symbol && kIdentifier != p->symbol) {
      logger.error("expect a number or identifier {}", to_string(*p));
      return nullptr;
    }
    auto primary = ast_.CreateTerminal(move(*p));
    ++p;

    return primary;
  }
}

AstNode *ClikeParser::ParsePostfixExpr(TokenIterator &p) {
  auto primary = ParsePrimaryExpr(p);

  if (kInc == p->symbol || kDec == p->symbol) {
    auto postfix_op = ast_.CreateTerminal(move(*p));
    ++p;
    postfix_op->push_child_back(primary);
    return postfix_op;

  } else {
    return primary;
  }
}

AstNode *ClikeParser::ParsePosNegExpr(TokenIterator &p) {
  if (kAdd == p->symbol || kSub == p->symbol) {
    auto posneg_op = ast_.CreateTerminal(move(*p));
    ++p;
    auto postfix = ParsePostfixExpr(p);
    posneg_op->push_child_back(postfix);
    return posneg_op;
  } else {
    return ParsePostfixExpr(p);
  }
}

AstNode *ClikeParser::ParseMulDivExpr(TokenIterator &p) {
  auto curr_expr = ParsePosNegExpr(p);

  while (kMul == p->symbol || kDiv == p->symbol) {
    auto muldiv_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParsePosNegExpr(p);
    muldiv_op->push_child_back(curr_expr);
    muldiv_op->push_child_back(rhs_expr);
    curr_expr = muldiv_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseAddSubExpr(TokenIterator &p) {
  auto curr_expr = ParseMulDivExpr(p);

  while (kAdd == p->symbol || kSub == p->symbol) {
    auto addsub_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseMulDivExpr(p);
    addsub_op->push_child_back(curr_expr);
    addsub_op->push_child_back(rhs_expr);
    curr_expr = addsub_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseCompareExpr(TokenIterator &p) {
  auto curr_expr = ParseAddSubExpr(p);

  while (kLE == p->symbol || kGE == p->symbol
      || kLT == p->symbol || kGT == p->symbol) {
    auto compare_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseAddSubExpr(p);
    compare_op->push_child_back(curr_expr);
    compare_op->push_child_back(rhs_expr);
    curr_expr = compare_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseEquationExpr(TokenIterator &p) {
  auto curr_expr = ParseCompareExpr(p);

  while (kNE == p->symbol || kEQ == p->symbol) {
    auto equation_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseCompareExpr(p);
    equation_op->push_child_back(curr_expr);
    equation_op->push_child_back(rhs_expr);
    curr_expr = equation_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseAssignExpr(TokenIterator &p) {
  std::stack<AstNode*> assign_stack;
  auto first_expr = ParseEquationExpr(p);
  assign_stack.push(first_expr);

  while (kAssign == p->symbol) {
    auto assign_op = ast_.CreateTerminal(move(*p));
    assign_stack.push(assign_op);
    ++p;

    auto rhs_expr = ParseEquationExpr(p);
    assign_stack.push(rhs_expr);
  }

  AstNode *curr_node = assign_stack.top();
  assign_stack.pop();
  while (!assign_stack.empty()) {
    auto assign_op = assign_stack.top();
    assign_stack.pop();
    auto lhs_expr = assign_stack.top();
    assign_stack.pop();
    assign_op->push_child_back(lhs_expr);
    assign_op->push_child_back(curr_node);
    curr_node = assign_op;
  }
  return curr_node;
}

AstNode *ClikeParser::ParseCommaExpr(TokenIterator &p) {
  auto curr_expr   = ParseAssignExpr(p);

  while (kComma == p->symbol) {
    auto comma_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseAssignExpr(p);
    comma_op->push_child_back(curr_expr);
    comma_op->push_child_back(rhs_expr);
    curr_expr = comma_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseExpr(TokenIterator &p) {
  // return ParsePrimaryExpr(p);
  // return ParsePostfixExpr(p);
  // return ParsePosNegExpr(p);
  // return ParseMulDivExpr(p);
  // return ParseAddSubExpr(p);
  // return ParseCompareExpr(p);
  // return ParseEquationExpr(p);
  // return ParseAssignExpr(p);
  return ParseCommaExpr(p);
}

AstNode *ClikeParser::ParsePrintf(TokenIterator &p) {
  if (kPrintf != p->symbol) {
    logger.error("expect a printf {}", to_string(*p));
    return nullptr;
  }

  auto printf = ast_.CreatePrintf(move(*p));
  while (kSemicolon != p->symbol) {
    // skip all except ;
    ++p;
  }
  // skip ;
  ++p;
  return printf;
}

AstNode *ClikeParser::ParseIf(TokenIterator &p) {
  if (kIf != p->symbol) {
    logger.error("expect a if {}", to_string(*p));
    return nullptr;
  }

  auto if_root = ast_.CreateIfRoot(kIfRoot);

  while (kIf == p->symbol) {
    auto clause = ast_.CreateIfClause(move(*p));
    ++p;

    if (kLeftParen != p->symbol) {
      logger.error("expect a ( {}", to_string(*p));
      return nullptr;
    }
    ++p;

    auto head = ParseExpr(p);

    if (kRightParen != p->symbol) {
      logger.error("expect a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    auto body = ParseSubBlock(p);

    clause->push_child_back(head);
    clause->push_child_back(body);

    if_root->push_child_back(clause);

    if (kElse == p->symbol) {
      if (kIf == next(p)->symbol) {
        // skip else
        ++p;
      } else {
        auto else_clause = ast_.CreateIfClause(move(*p));
        auto else_body = ParseSubBlock(p);
        else_clause->push_child_back(else_body);
        if_root->push_child_back(else_clause);
        break;
      }
    }
  }
  return if_root;
}

AstNode *ClikeParser::ParseLine(TokenIterator &p) {
  return nullptr;
}

AstNode *ClikeParser::ParseBlockBody(TokenIterator &p) {
  return nullptr;
}

AstNode *ClikeParser::ParseSubBlock(TokenIterator &p) {
  if (kLeftBrace == p->symbol) {
    ++p;

    auto block = ParseBlockBody(p);

    if (kRightParen != p->symbol) {
      logger.error("expect righ-brace {}", to_string(*p));
      return nullptr;
    }
    ++p;

    return block;

  } else {
    return ParseLine(p);
  }
}
