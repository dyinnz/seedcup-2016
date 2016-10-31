//
// Created by coder on 16-10-24.
//

#pragma once

#include <vector>
#include "token.h"
#include "mem_manager.h"

/**
 * @brief   The node of Abstract Syntax Tree.
 *
 * @details If the node is not attached to a token, then the str will be empty,
 *          and the row and column will be SIZE_MAX.
 *          If so, then the str, row and the column will extracted
 *          from the token.
 *
 *          The node should created by grammar parser, and the meanings of a
 *          node depends on the symbol and back-end of compiler.
 */
class AstNode {
 public:
  AstNode(const Symbol &symbol) : symbol_(symbol) {}
  AstNode(Token &&token)
      : symbol_(token.symbol),
        str_(std::move(token.str)),
        row_(token.row),
        column_(token.column) {}

  /**
   * @brief     Auxiliary function for debuging & printing
   * @return    A string representing this node
   */
  std::string to_string() const {
    std::ostringstream oss;
    if (symbol_.IsTerminal()) {
      oss << "Node/T { P(" << row_ << ',' << column_ << "), " << symbol_
          << ", " << str_ << "}";
    } else {
      oss << "Node/NT { " << symbol_ << " }";
    }
    return oss.str();
  }

  /**
   * @brief     Push a new child to this node
   * @param child   new child
   */
  void push_child_back(AstNode *child) {
    children_.push_back(child);
  }

  /**
   * @return    All children of this node
   */
  const std::vector<AstNode *> &children() const {
    return children_;
  }

  /**
   * @return    The grammar symbol of this node
   */
  const Symbol &symbol() const {
    return symbol_;
  }

  /**
   * @return    The string extracted from source code if the node attached to
   *            a token
   */
  const std::string &text() const {
    return str_;
  }

  /**
   * @return    The row of the token attached in source code
   */
  size_t row() const {
    return row_;
  }

  /**
    * @return    The row of the token attached in source code
    */
  size_t column() const {
    return column_;
  }

 private:
  std::vector<AstNode *> children_;
  Symbol symbol_;
  std::string str_;
  size_t row_{SIZE_MAX};
  size_t column_{SIZE_MAX};
};

/**
 * @brief   The Abstract Syntax Tree of the program
 *
 * @details The tree is used to contains all the node created in the parsing
 *          process. And it could manager the node momery, so could only be
 *          moved instead of coping.
 */
class Ast {
 public:
  Ast() {}
  Ast(const Ast &) = delete;
  Ast &operator=(const Ast &) = delete;
  Ast(Ast &&) = default;
  Ast &operator=(Ast &&) = default;

  /**
   * @brief     Release all the node that the ast hold
   */
  ~Ast() {
    for (auto node : node_manager_) {
      delete node;
    }
    node_manager_.clear();
  }

  /**
   * @brief     Create a node which holds terminal symbol attached a token
   * @param token The token extracted from source code
   * @return    A ast node
   */
  AstNode *CreateTerminal(Token &&token) {
    auto p = new AstNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  /**
   * @brief     Create a symbol which holds non-terminal symbol
   * @param symbol  The type of the non-terminal symbol
   * @return    A ast node
   */
  AstNode *CreateNonTerminal(const Symbol &symbol) {
    auto p = new AstNode(symbol);
    node_manager_.push_back(p);
    return p;
  }

  /**
   * @brief     Set the root node of the ast
   * @param root The root node
   */
  void set_root(AstNode *root) {
    root_ = root;
  }

  /**
   * @return  the root node of ast
   */
  AstNode *root() {
    return root_;
  }

 private:
  AstNode *root_{nullptr};
  std::vector<AstNode *> node_manager_;
};

/**
 * @brief   Auxililary function that prints ast node recursively
 * @param node  Current root node of a (sub) ast
 * @param deep  The deep of this node
 */
void PrintAstNode(AstNode *node, int deep = 0);

