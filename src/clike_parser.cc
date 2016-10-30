//
// Created by coder on 16-10-24.
//

#include <iterator>
#include <unordered_set>
#include "simplelogger.h"
#include "clike_grammar.h"
#include "clike_parser.h"

using namespace clike_grammar;
using namespace simple_logger;

using std::move;

/**
 * @brief   This is a simple launcher function. It do some simple work.
 * @see     clike_parser.h
 */
Ast ClikeParser::Parse(std::vector<Token> &tokens) {
  // push a sentry in case of touch the ending iterator of tokens
  tokens.push_back(kEofToken);

  // launch the parsing by call ParseBlockBody(): Start -> BlockBody
  auto p = tokens.begin();
  auto block = ParseBlockBody(p);

  // for debugging
  PrintAstNode(block);

  // set the root and
  ast_.set_root(block);
  return move(ast_);
}

/**
 * @param p Token position
 * @return  A node of declaration or definition
 * @brief   E.g. int x = 1, y, z = 0;
 *          Part 1: int
 *          Part 2: first identifier or assignment: x = 1
 *          Part 3: rest identifier or assignment: , y , z = 0
 *          Part 4: ;
 */
AstNode *ClikeParser::ParseTypeHead(TokenIterator &p) {
  auto parse_decl_def = [this](TokenIterator &p) -> AstNode * {
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

  // Part 1: int
  if (kInt != p->symbol) {
    func_error(logger, "expect a type {}", to_string(*p));
    return nullptr;
  }
  auto decl_node = ast_.CreateTerminal(move(*p));
  ++p;

  // Part 2: first declaration or definition
  auto sub_node = parse_decl_def(p);
  if (!sub_node) {
    func_log(logger, "expect a declaration or definition {}", to_string(*p));
    return nullptr;
  }
  decl_node->push_child_back(sub_node);

  // Part 3: rest declaration or definition
  while (kSemicolon != p->symbol) {
    // skip ,
    if (kComma != p->symbol) {
      func_log(logger, "expect a , {}", to_string(*p));
      return nullptr;
    }
    ++p;

    if (nullptr == (sub_node = parse_decl_def(p))) {
      func_log(logger, "expect a declaration or definition {}",
               to_string(*p));
      return nullptr;
    }
    decl_node->push_child_back(sub_node);
  }

  // Part 4: skip ;
  ++p;
  return decl_node;
}

/**
 * @param p Token position
 * @return  A node of printf statement
 * @brief   E.g. printf("hello world %d", expr);
 *          Part 1: printf (
 *          Part 2: "hello world"
 *          Part 3: , expr or epsilon
 *          Part 4: );
 */
AstNode *ClikeParser::ParsePrintf(TokenIterator &p) {
  // TODO: there are some corner cases
  // Part 1: printf (
  if (kPrintf != p->symbol) {
    func_error(logger, "expect a printf {}", to_string(*p));
    return nullptr;
  }
  auto printf = ast_.CreateTerminal(move(*p));
  ++p;

  if (kLeftParen != p->symbol) {
    func_error(logger, "expect a ( {}", to_string(*p));
    return nullptr;
  }
  ++p;

  // Part 2: a string
  if (kString != p->symbol) {
    func_error(logger, "expect a string {}", to_string(*p));
    return nullptr;
  }
  auto str = ast_.CreateTerminal(move(*p));
  ++p;
  printf->push_child_back(str);

  if (kComma == p->symbol) {
    // Part 3: , expr
    ++p; // skip the first comma

    auto expr = ParseExpr(p);
    printf->push_child_back(expr);

  } else if (kRightParen == p->symbol) {
    // Part 4: epsilon, do nothing
  } else {
    func_error(logger, "expect a , or ) {}", to_string(*p));
    return nullptr;

  }

  // Part 4: );
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

/**
 * @param p Token position
 * @return  A node of expression statement
 * @brief   ExprStmt -> PrintfStmt | Expr ";"
 */
AstNode *ClikeParser::ParseExprStmt(TokenIterator &p) {
  // TODO: move Printf Expr to Primary Expr
  AstNode *expr = nullptr;
  if (kPrintf == p->symbol) {
    expr = ParsePrintf(p);
  } else {
    expr = ParseExpr(p);
    if (kSemicolon != p->symbol) {
      func_error(logger, "expect a ; {}", to_string(*p));
      return nullptr;
    }
    ++p;
  }

  return expr;
}

/**
 * @param p Token position
 * @return  A node of expresssion that may suffix with + -
 * @brief   E.g. this_is_a_var
 *               999
 *               ( a + b )
 */
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

/**
 * @param p Token position
 * @return  A node of expresssion that may suffix with + -
 * @brief   E.g. +a, -100
 */
AstNode *ClikeParser::ParsePostfixExpr(TokenIterator &p) {
  auto primary = ParsePrimaryExpr(p);

  if (kInc == p->symbol || kDec == p->symbol) {
    // postfix with ++ --
    auto postfix_op = ast_.CreateTerminal(move(*p));
    ++p;
    postfix_op->push_child_back(primary);
    return postfix_op;

  } else {
    // no postfix
    return primary;
  }
}

/**
 * @param p Token position
 * @return  A node of expresssion that may suffix with + -
 * @brief   E.g. +a, -100
 */
AstNode *ClikeParser::ParsePosNegExpr(TokenIterator &p) {
  if (kAdd == p->symbol || kSub == p->symbol) {
    // suffix with + -
    auto posneg_op = ast_.CreateTerminal(move(*p));
    ++p;
    auto postfix = ParsePostfixExpr(p);
    posneg_op->push_child_back(postfix);
    return posneg_op;

  } else {
    // no suffix
    return ParsePostfixExpr(p);
  }
}

/**
 * @param p Token position
 * @return  A node of expresssion that may contain * /
 * @brief   E.g. a * b / c
 */
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

/**
 * @param p Token position
 * @return  A node of expresssion that may contain + -
 * @brief   E.g. a + b - c
 */
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

/**
 * @param p Token position
 * @return  A node of expresssion that may contain comparation
 * @brief   E.g. a < b >= c
 */
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

/**
 * @param p Token position
 * @return  A node of expresssion that may contain == !=
 * @brief   E.g. a == b != c
 */
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

/**
 * @param p Token position
 * @return  A node of expression that may contain assignment
 * @brief   E.g. a = b = c
 */
AstNode *ClikeParser::ParseAssignExpr(TokenIterator &p) {
  std::stack<AstNode *> assign_stack;
  // First: expr
  auto first_expr = ParseEquationExpr(p);
  assign_stack.push(first_expr);

  // Rest: , expr ...
  while (kAssign == p->symbol) {
    auto assign_op = ast_.CreateTerminal(move(*p));
    assign_stack.push(assign_op);
    ++p;

    auto rhs_expr = ParseEquationExpr(p);
    assign_stack.push(rhs_expr);
  }

  // Pop from stack
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

/**
 * @param p Token position
 * @return  A node of expression that may contain comma
 * @brief   E.g. expr1, expr2, expr3
 */
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

/**
 * @param p Token position
 * @return  A node of expression
 * @brief   There has 8 level of precedence
 */
AstNode *ClikeParser::ParseExpr(TokenIterator &p) {
  // return ParsePrimaryExpr(p);
  // return ParsePostfixExpr(p);
  // return ParsePosNegExpr(p);
  // return ParseMulDivExpr(p);
  // return ParseAddSubExpr(p);
  // return ParseCompareExpr(p);
  // return ParseEquationExpr(p);
  // return ParseAssignExpr(p);

  // check the first token
  static std::unordered_set<Symbol> expr_firsts{
      kAdd, kSub, kLeftParen, kIdentifier, kNumber,
  };
  if (expr_firsts.end() == expr_firsts.find(p->symbol)) {
    func_notice(logger, "expect +, -, (, id, number {}", to_string(*p));
  }
  return ParseCommaExpr(p);
}

/**
 * @param p Token position
 * @return  A node of a single statement
 *
 * @brief   Parsing single statement.
 *
 * @details A single statement could be a single expression statement,
 *          a block around
 */
AstNode *ClikeParser::ParseSingleStmt(TokenIterator &p) {
  auto parse_break = [this](TokenIterator &p) -> AstNode * {
    auto break_node = ast_.CreateTerminal(move(*p));
    ++p;
    if (kSemicolon != p->symbol) {
      func_error(logger, "expect a ; {}", to_string(*p));
      return nullptr;
    }
    ++p;
    return break_node;
  };

  switch (p->symbol.ID()) {
    case kIntID:
      return ParseTypeHead(p);
    case kSemicolonID:
      return ast_.CreateTerminal(move(*p++));
    case kBreakID:
      return parse_break(p);
    case kDoID:
      return ParseDoWhile(p);
    case kWhileID:
      return ParseWhile(p);
    case kForID:
      return ParseFor(p);
    case kIfID:
      return ParseIf(p);
    case kLeftBraceID:
      return ParseBraceBlock(p);
    default:
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

AstNode *ClikeParser::ParseBlockBody(TokenIterator &p) {
  auto block = ast_.CreateNonTerminal(kBlock);

  while (true) {
    // check whether there are some LFs
    while (kLFSymbol == p->symbol) {
      func_notice(logger, "unexpected LF {}", to_string(*p));
      ++p;
    }
    if (kEofSymbol == p->symbol || kRightBrace == p->symbol) {
      break;
    }
    block->push_child_back(ParseSingleStmt(p));
  }

  return block;
}

/**
 * @param p Token position
 * @return  A node of if-root
 */
AstNode *ClikeParser::ParseIf(TokenIterator &p) {
  auto parse_if_clause = [this](TokenIterator &p) -> AstNode * {
    // if token
    auto clause = ast_.CreateTerminal(move(*p));
    ++p;

    if (kLeftParen != p->symbol) {
      func_error(logger, "expect a ( {}", to_string(*p));
      return nullptr;
    }
    ++p;

    // Condition expression
    auto head = ParseExpr(p);

    if (kRightParen != p->symbol) {
      func_error(logger, "expect a ) {}", to_string(*p));
      return nullptr;
    }
    ++p;

    // Executable body
    auto body = ParseSingleStmt(p);

    clause->push_child_back(head);
    clause->push_child_back(body);
    return clause;
  };

  // check
  if (kIf != p->symbol) {
    func_error(logger, "expect a if {}", to_string(*p));
    return nullptr;
  }

  auto if_root = ast_.CreateNonTerminal(kIfRoot);

  while (kIf == p->symbol) {
    auto clause = parse_if_clause(p);
    if_root->push_child_back(clause);

    if (kElse == p->symbol) {
      if (kIf == next(p)->symbol) {
        // skip else
        ++p;
      } else {
        auto else_clause = ast_.CreateTerminal(move(*p));
        ++p;

        auto else_body = ParseSingleStmt(p);
        else_clause->push_child_back(else_body);
        if_root->push_child_back(else_clause);
        break;
      }
    } else {
      // the next token does not belong to if
      break;
    }
  }
  return if_root;
}

/**
 * @param p Token position
 * @return  A node of for
 *
 * @brief   Parsing for structure
 * @details There are three pseudo expression in for: Init, Condition, Step.
 *          Init could be any statement,
 *          Condition could be only be expr statement,
 *          Step could to be only be expr statement.
 */
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

  // first expression with ;
  auto init = ParseSingleStmt(p);

  // second expression with ;
  AstNode *condition = nullptr;
  if (kSemicolon == p->symbol) {
    // empty condition
    condition = ast_.CreateTerminal(Token(*p));
    ++p;
  } else {
    // expression condition
    condition = ParseExpr(p);
    if (kSemicolon != p->symbol) {
      func_error(logger, "expect a ; {}", to_string(*p));
      return nullptr;
    }
    ++p;
  }

  // third expression with )
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

  auto body = ParseSingleStmt(p);

  // construct for node
  for_node->push_child_back(init);
  for_node->push_child_back(condition);
  for_node->push_child_back(step);
  for_node->push_child_back(body);
  return for_node;
}

/**
 * @param p Token position
 * @return  A node of while
 *          while (expr) {      // Part 1
 *            multi-statement   // Part 2
 *          }
 */
AstNode *ClikeParser::ParseWhile(TokenIterator &p) {
  // Part 1
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

  // Part 2
  auto body = ParseSingleStmt(p);

  // construct while node
  while_node->push_child_back(condition);
  while_node->push_child_back(body);
  return while_node;
}

/**
 * @param p Token position
 * @return  A node of do while
 *          do {                    // Part 1
 *            multi-statement       // Part 2
 *          } while (expr);         // Part 3
 */
AstNode *ClikeParser::ParseDoWhile(TokenIterator &p) {
  // Part 1
  if (kDo != p->symbol) {
    func_error(logger, "expect a do {}", to_string(*p));
    return nullptr;
  }
  auto do_node = ast_.CreateTerminal(move(*p));
  ++p;

  // Part 2
  auto body = ParseBraceBlock(p);

  // Part 3
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

  // construct the do-while node
  do_node->push_child_back(body);
  do_node->push_child_back(condition);
  return do_node;
}
