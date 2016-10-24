//
// Created by coder on 16-10-24.
//

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
      next = ParseAssign(p);
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
    logger.error("There should be a type {}", to_string(*p));
    return nullptr;
  }
  auto decl_node = ast_.CreateDeclare(move(*p));
  ++p;

  auto parse_decl_def = [&]() -> AstNode * {
    if (kIdentifier != p->symbol) {
      logger.error("There should be a identifier {}", to_string(*p));
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

      auto expr = ParseExpr(p);

      assign->push_child_back(identifier);
      assign->push_child_back(expr);
      return assign;
    }
  };

  // first declaration or definition
  auto sub_node = parse_decl_def();
  if (!sub_node) {
    logger.log("There should be a declaration or definition {}", to_string(*p));
    return nullptr;
  }
  decl_node->push_child_back(sub_node);

  while (kSemicolon != p->symbol) {
    // skip ,
    if (kComma != p->symbol) {
      logger.log("There should be a , {}", to_string(*p));
      continue;
    } else {
      ++p;
    }

    if (nullptr == (sub_node = parse_decl_def())) {
      logger.log("There should be a declaration or definition {}",
                 to_string(*p));
      return nullptr;
    }
    decl_node->push_child_back(sub_node);
  }

  // skip ;
  ++p;
  return decl_node;
}

AstNode *ClikeParser::ParseDeclare(TokenIterator &p) {
  // type int
  if (kInt != p->symbol) {
    return nullptr;
  }
  auto decl_node = ast_.CreateDeclare(move(*p));
  ++p;

  // first declaration
  if (kIdentifier != p->symbol) {
    return nullptr;
  }
  decl_node->push_child_back(ast_.CreateTerminal(move(*p)));
  ++p;

  while (kSemicolon != p->symbol) {
    if (kComma != p->symbol) {
      continue;
    }
    // skip comma: ,
    ++p;

    if (kIdentifier != p->symbol) {
      continue;
    }
    // add other declaration
    decl_node->push_child_back(ast_.CreateTerminal(move(*p)));
    ++p;
  }

  // skip ;
  ++p;
  return decl_node;
}

// TODO: assign list
AstNode *ClikeParser::ParseAssign(TokenIterator &p) {
  // identifier
  if (kIdentifier != p->symbol) {
    logger.error("There should be a identifier {}", to_string(*p));
    return nullptr;
  }
  auto identifier = ast_.CreateTerminal(move(*p));
  ++p;

  // =
  if (kAssign != p->symbol) {
    logger.error("There should be a = {}", to_string(*p));
    return nullptr;
  }
  auto assign = ast_.CreateAssign(move(*p));
  ++p;

  // expr
  auto expr = ParseExpr(p);

  assign->push_child_back(identifier);
  assign->push_child_back(expr);

  if (kSemicolon != p->symbol) {
    logger.error("There should be a ; {}", to_string(*p));
    return nullptr;
  }
  ++p;
  return assign;
}

AstNode *ClikeParser::ParseExpr(TokenIterator &p) {
  auto curr_expr = ParseTermExpr(p);

  while (kAdd == p->symbol || kSub == p->symbol) {
    auto addsub_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseTermExpr(p);
    addsub_op->push_child_back(curr_expr);
    addsub_op->push_child_back(rhs_expr);
    curr_expr = addsub_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseTermExpr(TokenIterator &p) {
  auto curr_expr = ParseUnaryExpr(p);

  while (kMul == p->symbol || kDiv == p->symbol) {
    auto muldiv_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto rhs_expr = ParseUnaryExpr(p);
    muldiv_op->push_child_back(curr_expr);
    muldiv_op->push_child_back(rhs_expr);
    curr_expr = muldiv_op;
  }

  return curr_expr;
}

AstNode *ClikeParser::ParseUnaryExpr(TokenIterator &p) {
  if (kInc == p->symbol || kDec == p->symbol) {
    // begin with ++/--
    auto unary_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto primary = ParsePrimaryExpr(p);
    unary_op->push_child_back(primary);
    return unary_op;

  } else if (kAdd == p->symbol || kSub == p->symbol) {
    // begin with +/-
    auto head_op = ast_.CreateTerminal(move(*p));
    ++p;

    auto primary = ParsePrimaryExpr(p);
    if (kInc == p->symbol || kDec == p->symbol) {
      // end with ++/--
      auto tail_op = ast_.CreateTerminal(move(*p));
      ++p;
      // push twice
      tail_op->push_child_back(primary);
      tail_op->push_child_back(primary);
      head_op->push_child_back(tail_op);

    } else {
      // end with nothing
      head_op->push_child_back(primary);
    }
    return head_op;

  } else {
    // priamry expr
    auto primary = ParsePrimaryExpr(p);
    if (kInc == p->symbol || kDec == p->symbol) {
      auto tail_op = ast_.CreateTerminal(move(*p));
      ++p;
      // push twice
      tail_op->push_child_back(primary);
      tail_op->push_child_back(primary);
      return tail_op;

    } else {
      return primary;
    }
  }
}

AstNode *ClikeParser::ParsePrimaryExpr(TokenIterator &p) {
  if (kLeftParen == p->symbol) {
    // begin with (
    ++p;

    auto expr = ParseExpr(p);

    // end with )
    if (kRightParen != p->symbol) {
      logger.error("There should be a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    return expr;

  } else {
    // begin with identifier & positive number
    if (kNumber != p->symbol && kIdentifier != p->symbol) {
      logger.error("There should be a number or identifier {}", to_string(*p));
      return nullptr;
    }
    auto primary = ast_.CreateTerminal(move(*p));
    ++p;

    return primary;
  }
}

AstNode *ClikeParser::ParsePrintf(TokenIterator &p) {
  if (kPrintf != p->symbol) {
    logger.error("There should be a printf {}", to_string(*p));
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
    logger.error("There should be a if {}", to_string(*p));
    return nullptr;
  }

  auto
}
