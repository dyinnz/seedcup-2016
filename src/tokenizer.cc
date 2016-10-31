// // Created by Dyinnz on 16-9-5.
//

#include "tokenizer.h"
#include "simplelogger.h"

using std::vector;
using std::string;
using std::move;
using std::pair;

extern simple_logger::BaseLogger logger;

bool Tokenizer::MatchString(const char *p, const std::string &str) {
  if (str.empty()) {
    return false;
  }

  size_t i = 0;
  while (i < str.size() && p + i < end_) {
    if (p[i] != str[i]) {
      return false;
    }
    i += 1;
  }
  return (p + i) <= end_ && i == str.size();
}

const char *Tokenizer::SkipComment(const char *p) {
  if (MatchString(p, line_comment_start_)) {
    p += line_comment_start_.size();

    while (p < end_ && *p != '\n') {
      p += 1;
    }
    if (p < end_) {
      // get a LF
      curr_row_ += 1;
      curr_row_pos_ = p + 1;
      return p + 1;
    } else {
      // get EOF
      return p;
    }

  } else if (MatchString(p, block_comment_start_)) {
    p += block_comment_start_.size();

    while (p < end_ && !MatchString(p, block_comment_end_)) {
      if (*p == '\n') {
        // get a LF
        curr_row_ += 1;
        curr_row_pos_ = p + 1;
      }
      p += 1;
    }
    if (p < end_) {
      return p + block_comment_end_.size();
    } else {
      return p;
    }

  } else {
    return p;
  }
}

Token Tokenizer::GetNextToken(const char *&p) {
  assert(p < end_);

  Token longest_token = kErrorToken;

  const DFANode *curr_node = token_dfa_->start();

  const char *s = p;
  while (s != end_) {
    const DFANode *next_node = curr_node->GetNextNode(*s);

    if (next_node) {
      curr_node = next_node;
      s += 1;
      // logger.debug("{}", to_string(*curr_node));

      if (curr_node->IsEnd()) {
        int priority = curr_node->priority();
        longest_token.symbol = priority_to_symbol_[priority];
      }

    } else {
      break;
    }
  }

  longest_token.text = std::string(p, s);
  longest_token.row = curr_row_;
  longest_token.column = p - curr_row_pos_;
  p = s;

  // logger.debug("{}(): {}", __func__, to_string(longest_token));
  return longest_token;
}

bool Tokenizer::LexicalAnalyze(const string &s, vector<Token> &tokens) {
  return LexicalAnalyze(s.c_str(), s.c_str() + s.length(), tokens);
}

bool Tokenizer::LexicalAnalyze(const char *beg,
                               const char *end,
                               vector<Token> &tokens) {
  assert(token_dfa_);

  beg_ = beg;
  end_ = end;

  curr_row_ = 1;
  curr_row_pos_ = beg_;

  curr_ = beg;
  while (true) {
    const char *new_curr_ = curr_;
    do {
      curr_ = new_curr_;
      new_curr_ = SkipComment(curr_);
    } while (curr_ != new_curr_);

    if (curr_ >= end_) break;

    Token token = GetNextToken(curr_);

    // error
    if (token.symbol == kErrorSymbol) {
      logger.error("could not get next token at ({}, {})",
                   curr_row_,
                   curr_ - curr_row_pos_);
      return false;
    }
    // record line no.
    if (token.symbol == kLFSymbol) {
      curr_row_ += 1;
      curr_row_pos_ = curr_;
      token.text = "\\n";
    }
    // skip ignored token
    if (ignore_set_.end() == ignore_set_.find(token.symbol)) {
      if (!(token.symbol == kLFSymbol && !tokens.empty()
          && tokens.back().symbol == kLFSymbol)) {
        tokens.push_back(move(token));
      }
    }
  }

  return true;
}

TokenizerBuilder &
TokenizerBuilder::SetPatterns(const std::vector<TokenPattern> &patterns) {
  ResetPriority();

  RegexParser re_parser;
  vector<Symbol> priority_to_symbol(patterns.size());
  NFAComponent *result_comp = nullptr;

  for (auto &p : patterns) {
    auto &s = p.first;
    auto symbol = p.second;

    NFAComponent *comp = re_parser.ParseToNFAComponent(s);
    if (!comp) {
      logger.error("{}(): nullptr NFAComponent pointer", __func__);
      return *this;
    }

    int next_priority = NextPriority();
    comp->end()->set_priority(next_priority);
    priority_to_symbol[next_priority] = symbol;

    if (!result_comp) {
      result_comp = comp;
    } else {
      result_comp =
          re_parser.GetNFAManager().UnionWithMultiEnd(result_comp, comp);
    }
  }

  auto token_nfa = re_parser.GetNFAManager().BuildNFA(result_comp);
  if (!token_nfa) {
    is_error_ = true;
    return *this;
  }

  auto normal_dfa = ConvertNFAToDFA(token_nfa);
  if (!normal_dfa) {
    is_error_ = true;
    return *this;
  }

  auto min_dfa = MinimizeDFA(normal_dfa);
  if (!min_dfa) {
    is_error_ = true;
    return *this;
  }

  tokenizer_.priority_to_symbol_ = std::move(priority_to_symbol);
  tokenizer_.token_dfa_ = min_dfa;

  return *this;
}

Tokenizer TokenizerBuilder::Build() {
  if (tokenizer_.ignore_set_.empty()) {
    tokenizer_.ignore_set_.insert(kSpaceSymbol);
  }
  return std::move(tokenizer_);
}
