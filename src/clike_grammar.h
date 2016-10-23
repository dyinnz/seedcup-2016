//
// Created by coder on 16-10-23.
//

#pragma once

#include "tokenizer.h"

namespace clike_grammar {

// keyword
DECLARE_SYMBOL(kIf, 11)
DECLARE_SYMBOL(kElse, 12)
DECLARE_SYMBOL(kFor, 13)
DECLARE_SYMBOL(kBreak, 14)
DECLARE_SYMBOL(kWhile, 15)
DECLARE_SYMBOL(kDo, 16)

// int type
DECLARE_SYMBOL(kInt, 21)
// printf
DECLARE_SYMBOL(kPrintf, 22)

// ID
DECLARE_SYMBOL(kIdentifier, 23)
// literal
DECLARE_SYMBOL(kNumber, 24)
DECLARE_SYMBOL(kString, 25)

// one-char operator
DECLARE_CHAR_SYMBOL(kAdd, '+')
DECLARE_CHAR_SYMBOL(kSub, '-')
DECLARE_CHAR_SYMBOL(kMul, '*')
DECLARE_CHAR_SYMBOL(kDiv, '/')
DECLARE_CHAR_SYMBOL(kAssign, '=')
DECLARE_CHAR_SYMBOL(kSemicolon, ';')
DECLARE_CHAR_SYMBOL(kComma, ',')

DECLARE_CHAR_SYMBOL(kLT, '<')
DECLARE_CHAR_SYMBOL(kGT, '>')

DECLARE_CHAR_SYMBOL(kLeftParen, '(')
DECLARE_CHAR_SYMBOL(kRightParen, ')')
DECLARE_CHAR_SYMBOL(kLeftBrace, '{')
DECLARE_CHAR_SYMBOL(kRightBrace, '}')

// multi-char operator
DECLARE_SYMBOL(kInc, 31) // ++
DECLARE_SYMBOL(kDec, 32) // --

DECLARE_SYMBOL(kLE, 33) // <=
DECLARE_SYMBOL(kGE, 34) // >=
DECLARE_SYMBOL(kEQ, 35) // ==
DECLARE_SYMBOL(kNE, 36) // !=

Tokenizer BuilderClikeTokenizer();

} // end of namespace
