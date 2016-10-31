//
// Created by Dyinnz on 16-9-15.
//

#pragma once

#include <string>
#include <sstream>
#include "symbol.h"

/**
 * @brief   A token contains text extracted from source text, row and column
 *          number in source text.
 */
struct Token {
  Token(std::string text, const Symbol &symbol)
      : text(std::move(text)), symbol(symbol) {}

  bool operator==(const Token &rhs) const {
    return text == rhs.text && symbol == rhs.symbol;
  }

  bool operator!=(const Token &rhs) const {
    return !operator==(rhs);
  }

  std::string text;
  Symbol symbol;
  size_t row{0};
  size_t column{0};
};

/**
 * Predefine token which will be used frequently
 */
static const Token kErrorToken("kErrorToken", kErrorSymbol);
static const Token kEofToken("kEofToken", kEofSymbol);

/**
 * @brief   A helper function for debugging
 */
inline std::string to_string(const Token &token) {
  std::ostringstream oss;
  oss << "Token { P(" << token.row << ',' << token.column << "), "
      << token.symbol << ", " << token.text << " }";
  return oss.str();
}
