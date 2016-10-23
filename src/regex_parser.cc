//
// Created by coder on 16-9-3.
//

#include "simplelogger.h"
#include "regex_parser.h"

using std::string;
using std::shared_ptr;

extern simple_logger::BaseLogger logger;

namespace regular_expression {

/**
 * class REParser
 */

shared_ptr<DFA> RegexParser::ParseToDFA(const char *beg, const char *end) {
  beg_ = beg;
  end_ = end;

  auto nfa = nfa_manager_->BuildNFA(ParseUnion(beg));
  // PrintNFA(nfa->start(), nfa->size());

  auto normal = ConvertNFAToDFA(nfa);
  // PrintDFA(normal->start(), normal->size());

  auto minimum = MinimizeDFA(normal);
  // PrintDFA(minimum->start(), minimum->size());

  return minimum;
}

shared_ptr<DFA> RegexParser::ParseToDFA(const std::string &s) {
  return ParseToDFA(s.c_str(), s.c_str() + s.length());
}

NFAComponent *
RegexParser::ParseToNFAComponent(const char *beg, const char *end) {
  beg_ = beg;
  end_ = end;
  return ParseUnion(beg);
}

NFAComponent *RegexParser::ParseToNFAComponent(const string &s) {
  return ParseToNFAComponent(s.c_str(), s.c_str() + s.length());
}

NFAComponent *RegexParser::ParseUnion(const char *&p) {
  if (p >= end_) return nullptr;

  NFAComponent *result{nullptr};

  while (true) {
    NFAComponent *current = ParseConcatenate(p);

    if (result && current) {
      result = nfa_manager_->Union(current, result);

    } else if (!result && current) {
      result = current;

    } else {
      return nullptr;
    }

    // check what the next char is
    if ('|' == *p) {
      p += 1;

    } else if (end_ == p || ')' == *p) {
      break;

    } else {
      logger.error("{}(): unexpected end.", __func__);
      return nullptr;
    }
  }

  return result;
}

NFAComponent *RegexParser::ParseConcatenate(const char *&p) {
  assert(p < end_);

  NFAComponent *result{nullptr};

  while (true) {
    NFAComponent *current = ParseBasic(p);

    if (result && current) {
      result = nfa_manager_->Concatenate(result, current);

    } else if (!result && current) {
      result = current;

    } else {
      return nullptr;
    }

    if (end_ == p || ')' == *p || '|' == *p) {
      break;
    }
  }

  return result;
}

NFAComponent *RegexParser::ParseBasic(const char *&p) {
  assert(p < end_);

  NFAComponent *result{nullptr};

  switch (*p) {
    case '(':result = ParseGroup(p);
      break;

    case '[':result = ParseSet(p);
      break;

    case '.': {
      auto edge = nfa_manager_->CreateEdge();
      edge->set();
      result = nfa_manager_->CreateCompFromEdge(edge);
      p += 1;
    }
      break;

    case '\\':result = ParseEscape(p);
      break;

    default:
      // char
      result = nfa_manager_->CreateCompFromChar(*p);
      p += 1;
      break;
  }

  switch (*p) {
    case '*':result = nfa_manager_->KleenStar(result);
      p += 1;
      break;

    case '+':result = nfa_manager_->LeastOne(result);
      p += 1;
      break;

    case '?':result = nfa_manager_->Optional(result);
      p += 1;

    default:break;
  }

  return result;
}

NFAComponent *RegexParser::ParseGroup(const char *&p) {
  assert(p < end_);
  assert('(' == *p);
  p += 1;

  NFAComponent *result = ParseUnion(p);

  if (')' != *p) {
    logger.debug("{}(): {}", __func__, p);
    logger.error("{}(): right parenthesis dismatch", __func__);
    return nullptr;
  }

  p += 1;
  return result;
}

NFAComponent *RegexParser::ParseString(const char *&p) {
  // TODO
  assert(false);
  return nullptr;
}

NFAComponent *RegexParser::ParseEscape(const char *&p) {
  assert(p < end_);
  assert(*p == '\\');
  p += 1;

  NFAComponent *result = 0;
  result = nfa_manager_->CreateCompFromChar(*p);

  switch (*p) {
    case '\\':
    case '.':
    case '*':
    case '+':
    case '?':
    case '(':
    case ')':
    case '[':
    case ']':
    case '|':result = nfa_manager_->CreateCompFromChar(*p);
      break;

    case 'd':result = nfa_manager_->CreateCompFromRange('0', '9' + 1);
      break;

    case 'D': {
      auto edge = nfa_manager_->CreateEdge('0', '9' + 1);
      edge->flip();
      result = nfa_manager_->CreateCompFromEdge(edge);
    }
      break;

    case 's':result = nfa_manager_->CreateCompFromString(" \f\n\r\t\v");
      break;

    case 'S': {
      auto edge = nfa_manager_->CreateEdge(" \f\n\r\t\v");
      edge->flip();
      result = nfa_manager_->CreateCompFromEdge(edge);
    }
      break;

    case 'w': {
      auto edge = nfa_manager_->CreateEdge();
      edge->SetRange('a', 'z' + 1);
      edge->SetRange('A', 'Z' + 1);
      edge->SetRange('0', '9' + 1);
      edge->set('_');
      result = nfa_manager_->CreateCompFromEdge(edge);
    }
      break;

    case 'W': {
      auto edge = nfa_manager_->CreateEdge();
      edge->SetRange('a', 'z' + 1);
      edge->SetRange('A', 'Z' + 1);
      edge->SetRange('0', '9' + 1);
      edge->set('_');
      edge->flip();
      result = nfa_manager_->CreateCompFromEdge(edge);
    }
      break;

    default:
      logger.error("{}(): unrecognized escape char {}",
                   __func__,
                   int(*p));
      return nullptr;
  }

  p += 1;
  return result;
}

NFAComponent *RegexParser::ParseSet(const char *&p) {
  assert(p < end_);
  assert('[' == *p);
  p += 1;

  bool reverse = '^' == *p;
  if (reverse) {
    p += 1;
  }

  NFAEdge *edge = nfa_manager_->CreateEdge();
  for (; p < end_ && ']' != *p; ++p) {
    if ('-' != *p) {
      edge->set(*p);

    } else {
      if (p + 1 < end_ && ']' != *p && *(p - 1) < *(p + 1)) {
        edge->SetRange(*(p - 1), *(p + 1) + 1);

      } else {
        logger.error("{}(): wrong range", __func__);
        return nullptr;
      }
    }
  }

  if (reverse) {
    edge->flip();
  }

  if (']' != *p) {
    return nullptr;
  }
  p += 1;
  NFAComponent *result = nfa_manager_->CreateCompFromEdge(edge);
  return result;
}

} // end of namespace regular_expression
