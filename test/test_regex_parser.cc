//
// Created by coder on 16-9-4.
//

#define CATCH_CONFIG_MAIN
#define DEBUG

#include "catch.hpp"

#include "finite_automaton.h"
#include "regex_parser.h"
#include "simplelogger.h"

using std::shared_ptr;

using namespace simple_logger;
using namespace regular_expression;

BaseLogger logger;

TEST_CASE("concatenate simple string", "[Concatenate]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("abcd")};

  SECTION("matched") {
    REQUIRE(dfa->Match("abcd"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("abc"));
    REQUIRE_FALSE(dfa->Match("abcde"));
  }
}

TEST_CASE("union simple", "[Union]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("ab|xy|01")};

  SECTION("matched") {
    REQUIRE(dfa->Match("ab"));
    REQUIRE(dfa->Match("xy"));
    REQUIRE(dfa->Match("01"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("a"));
    REQUIRE_FALSE(dfa->Match("abxy"));
    REQUIRE_FALSE(dfa->Match("10"));
  }
}

TEST_CASE("concatenate postfix", "[Postfix]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("ab*c+d?e")};

  SECTION("matched") {
    REQUIRE(dfa->Match("ace"));
    REQUIRE(dfa->Match("abccde"));
    REQUIRE(dfa->Match("abbccce"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("abde"));
    REQUIRE_FALSE(dfa->Match("abcdde"));
    REQUIRE_FALSE(dfa->Match("abcd"));
  }
}

TEST_CASE("union postfix", "[Postfix]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("a*b+|c?d")};

  SECTION("matched") {
    REQUIRE(dfa->Match("ab"));
    REQUIRE(dfa->Match("b"));
    REQUIRE(dfa->Match("aabb"));
    REQUIRE(dfa->Match("cd"));
    REQUIRE(dfa->Match("d"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("a"));
    REQUIRE_FALSE(dfa->Match("ac"));
    REQUIRE_FALSE(dfa->Match("ccd"));
    REQUIRE_FALSE(dfa->Match("c"));
  }
}

TEST_CASE("group", "[Group]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("(a|b)*X|H(1|2+)?")};

  SECTION("matched") {
    REQUIRE(dfa->Match("aaaX"));
    REQUIRE(dfa->Match("bbX"));
    REQUIRE(dfa->Match("X"));
    REQUIRE(dfa->Match("ababbbaaX"));
    REQUIRE(dfa->Match("H"));
    REQUIRE(dfa->Match("H1"));
    REQUIRE(dfa->Match("H2"));
    REQUIRE(dfa->Match("H2222"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("XH"));
    REQUIRE_FALSE(dfa->Match("aabababa"));
    REQUIRE_FALSE(dfa->Match("H12"));
    REQUIRE_FALSE(dfa->Match("2222"));
  }
}

TEST_CASE("set", "[Set]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("[abc]+X[0-9]?[a-zH0-9]+")};

  SECTION("matched") {
    REQUIRE(dfa->Match("bX9aH9"));
    REQUIRE(dfa->Match("abcX5cHHH1230H"));
    REQUIRE(dfa->Match("abcabcXaH0aH0"));
    REQUIRE(dfa->Match("cX0123"));
  }

  SECTION("unmatched") {
    REQUIRE_FALSE(dfa->Match("dX0a"));
    REQUIRE_FALSE(dfa->Match("aXD"));
  }
}

TEST_CASE("meta char", "[MetaChar]") {
  RegexParser re_parser;
  shared_ptr<DFA> dfa{re_parser.ParseToDFA("\\d\\s\\w\\W\\\\")};

  SECTION("matched") {
    REQUIRE(dfa->Match("0\t_@\\"));
    REQUIRE(dfa->Match("9\nZ#\\"));
  }

  SECTION("unmath") {
    REQUIRE_FALSE(dfa->Match("0\t@@\\"));
    REQUIRE_FALSE(dfa->Match("0\t_@"));
    REQUIRE_FALSE(dfa->Match("a\t_@\\"));
    REQUIRE_FALSE(dfa->Match("00__\\"));
  }
}
