//
// Created by coder on 16-10-23.
//

#include "clike_grammar.h"

namespace clike_grammar {
// keyword
TERMINAL(kIf)
TERMINAL(kElse)
TERMINAL(kFor)
TERMINAL(kBreak)
TERMINAL(kWhile)
TERMINAL(kDo)

// int type
TERMINAL(kInt)
// printf
TERMINAL(kPrintf)

// ID
TERMINAL(kIdentifier)
// literal
TERMINAL(kNumber)
TERMINAL(kString)

// one-char operator
TERMINAL(kAdd)
TERMINAL(kSub)
TERMINAL(kMul)
TERMINAL(kDiv)
TERMINAL(kAssign)
TERMINAL(kSemicolon)
TERMINAL(kComma)

TERMINAL(kLT)
TERMINAL(kGT)

TERMINAL(kLeftParen)
TERMINAL(kRightParen)
TERMINAL(kLeftBrace)
TERMINAL(kRightBrace)

// multi-char operator
TERMINAL(kInc) // ++
TERMINAL(kDec) // --

TERMINAL(kLE) // <=
TERMINAL(kGE) // >=
TERMINAL(kEQ) // ==
TERMINAL(kNE) // !=

Tokenizer BuilderClikeTokenizer() {
  TokenizerBuilder builder;

  builder
      .SetLineComment("//")
      .SetBlockComment("/*", "*/")
      .SetIgnoreSet({kSpaceSymbol});

  builder.SetPatterns(
      {
          // space
          {"[ \v\t\f]", kSpaceSymbol},
          {"\r\n", kLFSymbol},
          // keyword
          {"if", kIf},
          {"else", kElse},
          {"for", kFor},
          {"break", kBreak},
          {"while", kWhile},
          {"do", kDo},
          // multi-char operator
          {R"(\+\+)", kInc},
          {"--", kDec},
          {"<=", kLE},
          {">=", kGE},
          {"==", kEQ},
          {"!=", kNE},
          // one-char operator
          {"{", kLeftBrace},
          {"}", kRightBrace},
          {R"(\()", kLeftParen},
          {R"(\))", kRightParen},
          {",", kComma},
          {";", kSemicolon},
          {"=", kAssign},
          {R"(\+)", kAdd},
          {"-", kSub},
          {R"(\*)", kMul},
          {"/", kDiv},
          {"<", kLT},
          {">", kGT},
          // ID & literal
          {R"(\d+)", kNumber},
          {R"("[^"]*")", kString},
          {R"(\w(\w|\d)*)", kIdentifier},
      }
  );

  return builder.Build();
}

}
