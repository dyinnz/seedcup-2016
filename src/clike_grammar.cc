//
// Created by coder on 16-10-23.
//

#include "clike_grammar.h"

namespace clike_grammar {

/**
 * Definition of teminal symbol
 */

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

/**
 * Definition of non-terminal symbol
 */
NON_TERMINAL(kBlock)
NON_TERMINAL(kIfRoot)


/**
 * @see clike_grammar.h
 */
Tokenizer BuilderClikeTokenizer() {
  TokenizerBuilder builder;

  builder
      .SetLineComment("//")
      .SetBlockComment("/*", "*/")
      /* Ingore all space symbol and LF */
      .SetIgnoreSet({kSpaceSymbol, kLFSymbol});

  builder.SetPatterns(
      {
          // space
          {"[ \v\t\f]", kSpaceSymbol},
          {"\r\n", kLFSymbol}, // dos-style LF

          // keyword
          {"if", kIf},
          {"else", kElse},
          {"for", kFor},
          {"break", kBreak},
          {"while", kWhile},
          {"do", kDo},
          {"int", kInt},
          {"printf", kPrintf},

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
          {R"((\d+)|(0(x|X)[a-f|A-F]+))", kNumber},  //TODO Hex Number (0xabcdef)
          {R"("([^"]|\\")*")", kString},
          {R"(\w(\w|\d)*)", kIdentifier},
      }
  );

  return builder.Build();
}

} // end of namespace clike_grammar
