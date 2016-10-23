//
// Created by coder on 16-9-15.
//

#pragma once

#include <string>
#include <sstream>
#include "symbol.h"

struct Token {
  Token(std::string str, const Symbol &symbol)
      : str(std::move(str)), symbol(symbol) {}

  bool operator==(const Token &rhs) const {
    return str == rhs.str && symbol == rhs.symbol;
  }

  bool operator!=(const Token &rhs) const {
    return !operator==(rhs);
  }

  std::string str;
  Symbol symbol;
  size_t row{0};
  size_t column{0};
};

static const Token kErrorToken("kErrorToken", kErrorSymbol);
static const Token kEofToken("kEofToken", kEofSymbol);

inline std::string to_string(const Token &token) {
  std::ostringstream oss;
  oss << "Token { " << token.symbol << ", P(" << token.row << ','
      << token.column << "), " << token.str << " }";
  return oss.str();
}
