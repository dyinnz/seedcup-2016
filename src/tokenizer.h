//
// Created by Dyinnz on 16-9-5.
//

#pragma once

#include "finite_automaton.h"
#include "regex_parser.h"

using namespace regular_expression;

#include "token.h"

typedef std::pair<std::string, Symbol> TokenPattern;

/**
 * @brief   A common Tokenizer .
 *
 * @details The tokenizer could be builded by some token pattern using
 *          regular expression. The earlier patterns have higher priority.
 *
 *          The tokenizer should only be created by TokenizerBuilder instead of
 *          creating directly.
 */
class Tokenizer {
 public:
  /**
   * @return the DFA inside used to match token
   */
  const DFA *GetTokenDFA() const {
    return &*token_dfa_;
  }

  const char *CurrentPos() {
    return curr_;
  }

  /**
   * @brief     Extracted next token on current position
   * @param p   current text position
   * @return    the token following current position
   */
  Token GetNextToken(const char *&p);

  /**
   * @param s       the source text
   * @param tokens  the tokens extracted
   * @return        whether succeed
   */
  bool LexicalAnalyze(const std::string &s,
                      std::vector<Token> &tokens);

  /**
   * @param beg     the begin position of source text
   * @param end     the end position of source text
   * @param tokens  the tokens extracted
   * @return        whether succeed
   */
  bool LexicalAnalyze(const char *beg,
                      const char *end,
                      std::vector<Token> &tokens);

 private:
  friend class TokenizerBuilder;

  /**
   * @brief         Auxiliary function, used to match the mark of comment
   * @param p       current position
   * @param str     string to be matched
   * @return        whether match
   */
  bool MatchString(const char *p, const std::string &str);

  /**
   * @param p   current position
   * @return    skip the liine and block comments
   */
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

/**
 * @brief   A helper class that build class Tokenizer.
 */
class TokenizerBuilder {
 public:
  bool IsError() {
    return is_error_;
  }

  /**
   * @brief             The index of pattern is its priority in tokenizing
   * @param patterns    A set of pairs of regex pattern and symbol
   * @return            this
   */
  TokenizerBuilder &SetPatterns(const std::vector<TokenPattern> &patterns);

  /**
   * @param ignore_set  the set of some ignored symbols
   * @return            this
   */
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

  /**
   * @brief     After calling this function, you should not call others.
   *            Because all the data has been moved.
   */
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

