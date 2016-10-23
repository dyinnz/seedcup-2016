//
// Created by coder on 16-9-5.
//

#pragma once

#include "finite_automaton.h"
#include "regex_parser.h"

using namespace regular_expression;

#include "token.h"

typedef std::pair<std::string, Symbol> TokenPattern;

class Tokenizer {
 public:
  const DFA *GetTokenDFA() const {
    return &*token_dfa_;
  }

  const char *CurrentPos() {
    return curr_;
  }

  Token GetNextToken(const char *&p);

  bool LexicalAnalyze(const std::string &s,
                      std::vector<Token> &tokens);

  bool LexicalAnalyze(const char *beg,
                      const char *end,
                      std::vector<Token> &tokens);

 private:
  friend class TokenizerBuilder;

  bool MatchString(const char *p, const std::string &str);
  const char *SkipComment(const char *p);

 private:
  std::shared_ptr<DFA> token_dfa_;
  std::vector<Symbol> priority_to_symbol_;
  std::unordered_set<Symbol> ignore_set_;

  /**
   * @brief     comment rules
   */
  std::string line_comment_start_;
  std::string block_comment_start_;
  std::string block_comment_end_;

  /**
   * @brief     position information
   */
  const char *beg_;
  const char *end_;
  const char *curr_;
  const char *curr_row_pos_;
  size_t curr_row_;
};

class TokenizerBuilder {
 public:
  bool IsError() {
    return is_error_;
  }

  TokenizerBuilder &SetPatterns(const std::vector<TokenPattern> &patterns);

  TokenizerBuilder &SetIgnoreSet(std::unordered_set<Symbol> ignore_set) {
    tokenizer_.ignore_set_ = std::move(ignore_set);
    return *this;
  }

  TokenizerBuilder &SetLineComment(const std::string &line_comment_start) {
    tokenizer_.line_comment_start_ = line_comment_start;
    return *this;
  }

  TokenizerBuilder &SetBlockComment(const std::string &block_comment_start,
                                    const std::string &block_comment_end) {
    tokenizer_.block_comment_start_ = block_comment_start;
    tokenizer_.block_comment_end_ = block_comment_end;
    return *this;
  }

  Tokenizer Build();

 private:
  void ResetPriority() {
    priority_index_ = 0;
  }

  int NextPriority() {
    return priority_index_++;
  }

 private:
  Tokenizer tokenizer_;
  int priority_index_{0};
  bool is_error_{false};
};

