//
// Created by Dyinnz on 16-10-24.
//

#pragma once

#include <stack>
#include "ast.h"

/**
 * @brief   C-like programming langague Parser
 *
 * @details A hand-written recursive descent parser. The parser is implemented
 *          according to the BNF-Grammar defined in the documentation.
 *          @see doc/bnf-grammar.exel
 *
 *          When Parse() is executed, the parser holds an ast inside, and
 *          move it to caller on returning. So that the parser is stateless,
 *          which could be called repeatly.
 */
class ClikeParser {
  typedef std::vector<Token>::iterator TokenIterator;

 public:
  /**
   * @brief   This is a simple launcher function. It do some simple work.
   *
   * @param tokens  Tokens extracted by tokenizer from sources code
   * @return        The ast
   */
  Ast Parse(std::vector<Token> &tokens);

 private:
  /**
   * All the parsing functions accept a TokenIterator refenrece, creating
   * a AstNode when finishing its work, and return it to called.
   *
   * All the parsing functions will modify the TokenIterator. After functions
   * finish recognizing their parts, they will leave the token unrecognized to
   * their caller.
   *
   * All the parsing functions will return nullptr if they do not accept
   * following tokens.
   */

  /**
   * Block statement & auxilary parsing function
   */
  AstNode *ParseBlockBody(TokenIterator &p); // the block without {}
  AstNode *ParseBraceBlock(TokenIterator &p); // the block with {}
  AstNode *ParseSingleStmt(TokenIterator &p);

  /**
   * Simple statement
   */
  AstNode *ParseTypeHead(TokenIterator &p); // int x, y = 1;
  AstNode *ParsePrintf(TokenIterator &p);
  AstNode *ParseExprStmt(TokenIterator &p); // any expr ending with ;

  /**
   * Expression, 9 levels of precedence
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

  /**
   * Control structure
   */
  AstNode *ParseIf(TokenIterator &p);
  AstNode *ParseFor(TokenIterator &p);
  AstNode *ParseWhile(TokenIterator &p);
  AstNode *ParseDoWhile(TokenIterator &p);

 private:
  Ast ast_;
};
