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

  auto p = curr_;
  auto block = ParseBlockBody(p);

  ast_.set_root(block);
  PrintAstNode(ast_.root());

  return move(ast_);
}

AstNode *ClikeParser::ParseTypeHead(TokenIterator &p) {
  // type int
  if (kInt != p->symbol) {
    func_error(logger, "expect a type {}", to_string(*p));
    return nullptr;
  }
  auto decl_node = ast_.CreateTerminal(move(*p));
  ++p;

  auto parse_decl_def = [&]() -> AstNode * {
    if (kIdentifier != p->symbol) {
      func_error(logger, "expect a identifier {}", to_string(*p));
      return nullptr;
    }
    auto identifier = ast_.CreateTerminal(move(*p));
    ++p;

    if (kAssign != p->symbol) {
      // There is a declaration
      return identifier;

    } else {
      // There is a definition
      auto assign = ast_.CreateTerminal(move(*p));
      ++p;

      // only allow assign expr
      auto expr = ParseAssignExpr(p);

      assign->push_child_back(identifier);
      assign->push_child_back(expr);
      return assign;
    }
  };

  // first declaration or definition
  auto sub_node = parse_decl_def();
  if (!sub_node) {
    func_log(logger, "expect a declaration or definition {}", to_string(*p));
    return nullptr;
  }
  decl_node->push_child_back(sub_node);

  while (kSemicolon != p->symbol) {
    // skip ,
    if (kComma != p->symbol) {
      func_log(logger, "expect a , {}", to_string(*p));
      continue;
    } else {
      ++p;
    }

    if (nullptr == (sub_node = parse_decl_def())) {
      func_log(logger, "expect a declaration or definition {}",
               to_string(*p));
      return nullptr;
    }
    decl_node->push_child_back(sub_node);
  }

  // skip ;
  ++p;
  return decl_node;
}

AstNode *ClikeParser::ParsePrintf(TokenIterator &p) {
  if (kPrintf != p->symbol) {
    func_error(logger, "expect a printf {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto printf = ast_.CreateTerminal(move(*p));

  if (kLeftParen != p->symbol) {
    func_error(logger, "expect a ( {}", to_string(*p));
    return nullptr;
  }
  ++p;

  if (kString != p->symbol) {
    func_error(logger, "expect a string {}", to_string(*p));
    return nullptr;
  }
  ++p;

  if (kComma != p->symbol) {
    func_error(logger, "expect a , {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto expr = ParseExpr(p);
  printf->push_child_back(expr);

  if (kRightParen != p->symbol) {
    func_error(logger, "expect a ) {}", to_string(*p));
    return nullptr;
  }
  ++p;

  if (kSemicolon != p->symbol) {
    func_error(logger, "expect a ; {}", to_string(*p));
    return nullptr;
  }
  ++p;

  return printf;
}

AstNode *ClikeParser::ParseExprStmt(TokenIterator &p) {
  auto expr = ParseExpr(p);
  if (kSemicolon != p->symbol) {
    func_error(logger, "expect a ; {}", to_string(*p));
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
      func_error(logger, "expect a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    return expr;

  } else {
    // begin with identifier & positive number
    if (kNumber != p->symbol && kIdentifier != p->symbol) {
      func_error(logger, "expect a number or identifier {}", to_string(*p));
      ++p;
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
  std::stack<AstNode *> assign_stack;
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
  auto curr_expr = ParseAssignExpr(p);

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

AstNode *ClikeParser::ParseLine(TokenIterator &p) {
  if (kInt == p->symbol) {
    return ParseTypeHead(p);
  } else if (kPrintf == p->symbol) {
    return ParsePrintf(p);
  } else if (kSemicolon == p->symbol) {
    return ast_.CreateTerminal(move(*p++));
  } else if (kBreak == p->symbol) {
    auto break_node = ast_.CreateTerminal(move(*p));
    ++p;
    if (kSemicolon != p->symbol) {
      func_error(logger, "expect a ; {}", to_string(*p));
      return nullptr;
    }
    ++p;
    return break_node;
  } else {
    return ParseExprStmt(p);
  }
}

AstNode *ClikeParser::ParseBraceBlock(TokenIterator &p) {
  if (kLeftBrace != p->symbol) {
    func_error(logger, "expect left-brace {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto block = ParseBlockBody(p);

  if (kRightBrace != p->symbol) {
    func_error(logger, "expect right-brace {}", to_string(*p));
    return nullptr;
  }
  ++p;

  return block;
}

AstNode *ClikeParser::ParseLineOrBlock(TokenIterator &p) {
  if (kLeftBrace == p->symbol) {
    return ParseBraceBlock(p);
  } else {
    return ParseLine(p);
  }
}

AstNode *ClikeParser::ParseBlockBody(TokenIterator &p) {
  auto block = ast_.CreateNonTerminal(kBlock);

  while (true) {
    while (kLFSymbol == p->symbol) {
      ++p;
    }
    if (kEofSymbol == p->symbol || kRightBrace == p->symbol) {
      break;
    }

    if (kDo == p->symbol) {
      auto do_node = ParseDoWhile(p);
      block->push_child_back(do_node);
    } else if (kWhile == p->symbol) {
      auto while_node = ParseWhile(p);
      block->push_child_back(while_node);
    } else if (kFor == p->symbol) {
      auto for_node = ParseFor(p);
      block->push_child_back(for_node);
    } else if (kIf == p->symbol) {
      auto if_node = ParseIf(p);
      block->push_child_back(if_node);
    } else if (kLeftBrace == p->symbol) {
      auto brace = ParseBraceBlock(p);
      block->push_child_back(brace);
    } else {
      auto line = ParseLine(p);
      block->push_child_back(line);
    }
  }

  return block;
}

AstNode *ClikeParser::ParseIf(TokenIterator &p) {
  if (kIf != p->symbol) {
    func_error(logger, "expect a if {}", to_string(*p));
    return nullptr;
  }

  auto if_root = ast_.CreateNonTerminal(kIfRoot);

  while (kIf == p->symbol) {
    auto clause = ast_.CreateTerminal(move(*p));
    ++p;

    if (kLeftParen != p->symbol) {
      func_error(logger, "expect a ( {}", to_string(*p));
      return nullptr;
    }
    ++p;

    auto head = ParseExpr(p);

    if (kRightParen != p->symbol) {
      func_error(logger, "expect a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    auto body = ParseLineOrBlock(p);

    clause->push_child_back(head);
    clause->push_child_back(body);

    if_root->push_child_back(clause);

    if (kElse == p->symbol) {
      ++p;
      if (kIf == p->symbol) {
        // skip
      } else {
        auto else_clause = ast_.CreateTerminal(move(*p));
        auto else_body = ParseLineOrBlock(p);
        else_clause->push_child_back(else_body);
        if_root->push_child_back(else_clause);
        break;
      }
    }
  }
  return if_root;
}

AstNode *ClikeParser::ParseFor(TokenIterator &p) {
  if (kFor != p->symbol) {
    func_error(logger, "expect a for {}", to_string(*p));
    return nullptr;
  }
  auto for_node = ast_.CreateTerminal(move(*p));
  ++p;

  if (kLeftParen != p->symbol) {
    func_error(logger, "expect a ( {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto init = ParseLine(p);

  AstNode *condition = nullptr;
  if (kSemicolon == p->symbol) {
    condition = ast_.CreateTerminal(Token(*p));
    ++p;
  } else {
    condition = ParseExpr(p);
    if (kSemicolon != p->symbol) {
      func_error(logger, "expect a ; {}", to_string(*p));
      return nullptr;
    }
    ++p;
  }

  AstNode *step = nullptr;
  if (kRightParen == p->symbol) {
    step = ast_.CreateTerminal(Token(*prev(p)));
  } else {
    step = ParseExpr(p);
  }

  if (kRightParen != p->symbol) {
    func_error(logger, "expect a ) {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto body = ParseLineOrBlock(p);

  for_node->push_child_back(init);
  for_node->push_child_back(condition);
  for_node->push_child_back(step);
  for_node->push_child_back(body);
  return for_node;
}

AstNode *ClikeParser::ParseWhile(TokenIterator &p) {
  if (kWhile != p->symbol) {
    func_error(logger, "expect a while {}", to_string(*p));
    return nullptr;
  }
  auto while_node = ast_.CreateTerminal(move(*p));
  ++p;

  if (kLeftParen != p->symbol) {
    func_error(logger, "expect a ( {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto condition = ParseExpr(p);

  if (kRightParen != p->symbol) {
    func_error(logger, "expect a ) {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto body = ParseLineOrBlock(p);

  while_node->push_child_back(condition);
  while_node->push_child_back(body);
  return while_node;
}

AstNode *ClikeParser::ParseDoWhile(TokenIterator &p) {
  if (kDo != p->symbol) {
    func_error(logger, "expect a do {}", to_string(*p));
    return nullptr;
  }
  auto do_node = ast_.CreateTerminal(move(*p));
  ++p;

  auto body = ParseBraceBlock(p);

  if (kWhile != p->symbol) {
    func_error(logger, "expect a while {}", to_string(*p));
    return nullptr;
  }
  ++p;

  if (kLeftParen != p->symbol) {
    func_error(logger, "expect a ( {}", to_string(*p));
    return nullptr;
  }
  ++p;

  auto condition = ParseExpr(p);

  if (kRightParen != p->symbol) {
    func_error(logger, "expect a ) {}", to_string(*p));
    return nullptr;
  }
  ++p;
  if (kSemicolon != p->symbol) {
    func_error(logger, "expect a ; {}", to_string(*p));
    return nullptr;
  }
  ++p;

  do_node->push_child_back(body);
  do_node->push_child_back(condition);
  return do_node;
}
