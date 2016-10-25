//
// Created by coder on 16-10-24.
//

#pragma once

#include <vector>
#include "token.h"
#include "mem_manager.h"

class AstNode {
 public:
  AstNode(const Symbol &symbol) : symbol_(symbol) {}
  AstNode(Token &&token)
      : symbol_(token.symbol),
        str_(std::move(token.str)),
        row_(token.row),
        column_(token.column) {}

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

  void push_child_back(AstNode *child) {
    children_.push_back(child);
  }

  const std::vector<AstNode *> &children() const {
    return children_;
  }

  const Symbol &symbol() const {
    return symbol_;
  }

  const std::string &str() const {
    return str_;
  }

  size_t row() const {
    return row_;
  }

  size_t column() const {
    return column_;
  }

 private:
  std::vector<AstNode *> children_;
  Symbol symbol_;
  std::string str_;
  size_t row_;
  size_t column_;
};

class Ast {
 public:
  AstNode *CreateTerminal(Token &&token) {
    auto p = new AstNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  AstNode *CreateNonTerminal(const Symbol &symbol) {
    auto p = new AstNode(symbol);
    node_manager_.push_back(p);
    return p;
  }

  void set_root(AstNode *root) {
    root_ = root;
  }

  AstNode *root() {
    return root_;
  }

 private:
  AstNode *root_{nullptr};
  std::vector<AstNode *> node_manager_;
};

void PrintAstNode(AstNode *node, int deep = 0);

