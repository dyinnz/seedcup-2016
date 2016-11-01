//
// Created by Dyinnz on 16-9-3.
//

/**
 * This is a regular expression parser using descent recursive parsing.
 *
 * User could parse a regular pattern to a DFA or a raw NFA commponent which
 * can be composed a complete NFA.
 */

#pragma once

#include "finite_automaton.h"
#include <iostream>

namespace regular_expression {

/**
 * @brief Regular Expression Parser, optimized for tokenizing
 */
class RegexParser {
 public:

  /**
   * @param nfa_manager NFA memory manager
   * @brief You could pass a NFA manager to this parser, or it will create one
   *        its own.
   */
  RegexParser(
      std::shared_ptr<NFAManager> nfa_manager = nullptr)
      : nfa_manager_(nfa_manager) {
    if (!nfa_manager_) {
      // nfa_manager_ = std::make_shared<NFAManager>();
      nfa_manager_ = std::shared_ptr<NFAManager>(new NFAManager);
    }
  }

  std::shared_ptr<DFA> ParseToDFA(const char *beg, const char *end);

  std::shared_ptr<DFA> ParseToDFA(const std::string &s);

  /**
   * @brief     In order to build a tokenizer, should not construct DFA
   *            directly. Only construct a simple NFA compoment, let caller to
   *            complete the rest construction and convertion.
   * @return    a NFA compoment
   */
  NFAComponent *ParseToNFAComponent(const char *beg, const char *end);

  NFAComponent *ParseToNFAComponent(const std::string &s);

  /**
   * @return Memory manager
   */
  NFAManager &GetNFAManager() {
    return *nfa_manager_;
  }

 private:
  /**
   * @param p   current string position
   * @return    NFA component
   */

  /**
   * @brief     a|b
   */
  NFAComponent *ParseUnion(const char *&p);

  /**
   * @brief     ab
   */
  NFAComponent *ParseConcatenate(const char *&p);

  /**
   * @brief     a
   */
  NFAComponent *ParseBasic(const char *&p);

  /**
   * @brief     (abc)
   */
  NFAComponent *ParseGroup(const char *&p);

  /**
   * @brief     [abc]
   */
  NFAComponent *ParseSet(const char *&p);

  /**
   * @deprecated
   */
  NFAComponent *ParseString(const char *&p);

  /**
   * @brief     \\w
   */
  NFAComponent *ParseEscape(const char *&p);

 private:
  const char *beg_{nullptr};
  const char *end_{nullptr};
  std::shared_ptr<NFAManager> nfa_manager_;
};

} // end of namespace regular_expression

