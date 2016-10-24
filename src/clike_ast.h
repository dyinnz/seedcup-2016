//
// Created by coder on 16-10-24.
//

#pragma once

#include <vector>
#include <deque>
#include "token.h"
#include "mem_manager.h"

class AstNode {
 public:
  virtual ~AstNode() = 0;

  virtual std::string to_string() const = 0;

  void push_child_back(AstNode *child) {
    children_.push_back(child);
  }

  void push_child_front(AstNode *child) {
    children_.push_front(child);
  }

  const std::deque<AstNode *> &children() const {
    return children_;
  }

 private:
  std::deque<AstNode *> children_;
};

class NonTerminalNode : public AstNode {
 public:
  virtual ~NonTerminalNode() {
  }

  NonTerminalNode(const Symbol &symbol) : symbol_(symbol) {}

  virtual std::string to_string() const {
    return "Node/NT { " + std::string(symbol_.str()) + " }";
  }

  const Symbol &symbol() const {
    return symbol_;
  }

 private:
  Symbol symbol_;
};

class TerminalNode : public AstNode {
 public:
  virtual ~TerminalNode() {
  }
  virtual std::string to_string() const {
    return "Node/T { " + ::to_string(token_) + " }";
  }

  TerminalNode(Token &&token) : token_(std::move(token)) {}

  const Symbol &symbol() const {
    return token_.symbol;
  }

  const std::string &str() const {
    return token_.str;
  }

  size_t row() const {
    return token_.row;
  }

  size_t column() const {
    return token_.column;
  }

 private:
  Token token_;
};

class DeclareNode : public TerminalNode {
 public:
  virtual ~DeclareNode() {
  }

  DeclareNode(Token &&token) : TerminalNode(std::move(token)) {}
};

class DefineNode : public NonTerminalNode {
 public:
  virtual ~DefineNode() {
  }

  DefineNode(const Symbol &symbol) : NonTerminalNode(symbol) {}
};

class AssignNode : public TerminalNode {
 public:
  AssignNode(Token &&token) : TerminalNode(std::move(token)) {}

};

class PrintfNode : public TerminalNode {
 public:
  PrintfNode(Token &&token) : TerminalNode(std::move(token)) {}

};

class ExprNode : public TerminalNode {
 public:
  ExprNode(Token &&token) : TerminalNode(std::move(token)) {}
};

class BlockNode : public NonTerminalNode {
 public:
  BlockNode(const Symbol &symbol) : NonTerminalNode(symbol) {}

};

class IfBlockNode : public NonTerminalNode {
 public:
  IfBlockNode(const Symbol &symbol) : NonTerminalNode(symbol) {}
};

class IfClauseNode : public TerminalNode {
 public:
  IfClauseNode(Token &&token) : TerminalNode(std::move(token)) {}

};

class ForNode : public TerminalNode {
 public:
  ForNode(Token &&token) : TerminalNode(std::move(token)) {}
};

class WhileNode : public TerminalNode {
 public:
  WhileNode(Token &&token) : TerminalNode(std::move(token)) {}
};

class DoWhileNode : public TerminalNode {
 public:
  DoWhileNode(Token &&token) : TerminalNode(std::move(token)) {}
};

class Ast {
 public:
  TerminalNode *CreateTerminal(Token &&token) {
    auto p = new TerminalNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  DeclareNode *CreateDeclare(Token &&token) {
    auto p = new DeclareNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  DefineNode *CreateDefine(const Symbol &symbol) {
    auto p = new DefineNode(symbol);
    node_manager_.push_back(p);
    return p;
  }

  AssignNode *CreateAssign(Token &&token) {
    auto p = new AssignNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  PrintfNode *CreatePrintf(Token &&token) {
    auto p = new PrintfNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  ExprNode *CreateExpr(Token &&token) {
    auto p = new ExprNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  BlockNode *CreateBlock(const Symbol &symbol) {
    auto p = new BlockNode(symbol);
    node_manager_.push_back(p);
    return p;
  }

  IfBlockNode *CreateIfBlock(const Symbol &symbol) {
    auto p = new IfBlockNode(symbol);
    node_manager_.push_back(p);
    return p;
  }

  IfClauseNode *CreateIfClause(Token &&token) {
    auto p = new IfClauseNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  ForNode *CreateFor(Token &&token) {
    auto p = new ForNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  WhileNode *CreateWhile(Token &&token) {
    auto p = new WhileNode(std::move(token));
    node_manager_.push_back(p);
    return p;
  }

  DoWhileNode *CreateDoWhile(Token &&token) {
    auto p = new DoWhileNode(std::move(token));
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

