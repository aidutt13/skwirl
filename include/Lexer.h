#pragma once

#include <ios>
#include <functional>
#include <sstream>
#include <unordered_map>

#include "Chars.h"

enum TokenType {
  NONE,
  EOB,
  IDENTIFIER,
  KEYWORD,
  INTEGER,
  FLOAT,
  STRING,
  CHAR,
  OPERATOR,
  PUNCTUATOR,
};

struct Token {
  TokenType type;
  std::string value;
  uint32_t row;
  uint32_t col;

  static const std::unordered_map<TokenType, std::string> typeNames;

  friend inline std::ostream &operator <<(std::ostream &os, const Token &token) {
    os << "Token(" << Token::typeNames.at(token.type) << ", `" << token.value << "`, " << token.row << ", " << token.col << ")";
    return os;
  }
};

class UnexpectedCharacterException : public std::exception {
  std::string m_message;
public:
  UnexpectedCharacterException(char c, uint32_t row, uint32_t col);
  const char* what() const noexcept override {
    return m_message.c_str();
  }
};

class Lexer {
private:
  std::stringstream m_data;
  uint32_t m_row = 0, m_col = 0, m_lastLineCol = 0;
  Token m_currentToken;

public:
  Lexer(std::basic_istream<char> &stream);

public:
  char peekChar();
  char nextChar();
  void putBackChar();
  bool eof();

private:
  void skipWhitespace();
  void skipComment();

  std::string readWhile(std::function<bool(const char *)> predicate);
  std::string readEscaped(char end);

  Token readNextToken();
  Token readNumberToken();
  Token readIdentifierToken();

public:
  Token nextToken();
  Token currentToken();
};