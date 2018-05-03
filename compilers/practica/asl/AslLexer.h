
// Generated from Asl.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"




class  AslLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, ASSIGN = 9, EQUAL = 10, NEQ = 11, GR = 12, GEQ = 13, LEQ = 14, 
    LESS = 15, PLUS = 16, MINUS = 17, MUL = 18, DIV = 19, MODUL = 20, AND = 21, 
    OR = 22, NOT = 23, TRUE = 24, FALSE = 25, VAR = 26, INT = 27, FLOAT = 28, 
    BOOL = 29, CHAR = 30, CHARVAL = 31, ARRAY = 32, IF = 33, THEN = 34, 
    ELSE = 35, ENDIF = 36, WHILE = 37, DO = 38, ENDWHILE = 39, FUNC = 40, 
    RETURN = 41, ENDFUNC = 42, READ = 43, WRITE = 44, ID = 45, INTVAL = 46, 
    FLOATVAL = 47, STRING = 48, COMMENT = 49, WS = 50
  };

  AslLexer(antlr4::CharStream *input);
  ~AslLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

