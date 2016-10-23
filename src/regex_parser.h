//
// Created by coder on 16-9-3.
//

#pragma once

#include "finite_automaton.h"
#include <iostream>

namespace regular_expression {

/**
 * Regular Expression Parser
 */

class RegexParser {
 public:
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

  NFAComponent *ParseToNFAComponent(const char *beg, const char *end);

  NFAComponent *ParseToNFAComponent(const std::string &s);

  NFAManager &GetNFAManager() {
    return *nfa_manager_;
  }

 private:
  NFAComponent *ParseUnion(const char *&p);

  NFAComponent *ParseConcatenate(const char *&p);

  NFAComponent *ParseBasic(const char *&p);

  NFAComponent *ParseGroup(const char *&p);

  NFAComponent *ParseSet(const char *&p);

  NFAComponent *ParseString(const char *&p);

  NFAComponent *ParseEscape(const char *&p);

 private:
  const char *beg_{nullptr};
  const char *end_{nullptr};
  std::shared_ptr<NFAManager> nfa_manager_;
};

} // end of namespace regular_expression

