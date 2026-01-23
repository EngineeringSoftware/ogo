
// Generated from Cypher.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  CypherLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, UNION = 40, ALL = 41, OPTIONAL = 42, MATCH = 43, UNWIND = 44, 
    AS = 45, MERGE = 46, ON = 47, CREATE = 48, SET = 49, DETACH = 50, DELETE = 51, 
    REMOVE = 52, CALL = 53, YIELD = 54, WITH = 55, RETURN = 56, DISTINCT = 57, 
    ORDER = 58, BY = 59, L_SKIP = 60, LIMIT = 61, ASCENDING = 62, ASC = 63, 
    DESCENDING = 64, DESC = 65, WHERE = 66, OR = 67, XOR = 68, AND = 69, 
    NOT = 70, IN = 71, STARTS = 72, ENDS = 73, CONTAINS = 74, IS = 75, NULL_ = 76, 
    COUNT = 77, ANY = 78, NONE = 79, SINGLE = 80, TRUE = 81, FALSE = 82, 
    EQ = 83, NEQ = 84, LT = 85, GT = 86, LTEQ = 87, GTEQ = 88, EXISTS = 89, 
    CASE = 90, ELSE = 91, END = 92, WHEN = 93, THEN = 94, StringLiteral = 95, 
    EscapedChar = 96, HexInteger = 97, DecimalInteger = 98, OctalInteger = 99, 
    HexLetter = 100, HexDigit = 101, Digit = 102, NonZeroDigit = 103, NonZeroOctDigit = 104, 
    OctDigit = 105, ZeroDigit = 106, ExponentDecimalReal = 107, RegularDecimalReal = 108, 
    CONSTRAINT = 109, DO = 110, FOR = 111, REQUIRE = 112, UNIQUE = 113, 
    MANDATORY = 114, SCALAR = 115, OF = 116, ADD = 117, DROP = 118, FILTER = 119, 
    EXTRACT = 120, UnescapedSymbolicName = 121, IdentifierStart = 122, IdentifierPart = 123, 
    EscapedSymbolicName = 124, SP = 125, WHITESPACE = 126, Comment = 127
  };

  explicit CypherLexer(antlr4::CharStream *input);

  ~CypherLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

