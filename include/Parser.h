#pragma once

#include "Lexer.h"
#include <variant>
#include <vector>


enum ASTType {
  NONE,
  EOB,
  NAME,
  INTEGER,
  FLOAT,
  STRING,
  CHAR,
  OPERATOR,
  PUNCTUATOR,

  PROG,
  FUNCTION,
  CALL,
  VAR,
};

namespace astid {
  #define AST_ID constexpr uint32_t
  
  AST_ID VALUE = 1;
  AST_ID PROG = 2;
  AST_ID FUNCTION_NAME = 3;
  AST_ID FUNCTION_PARAMS = 4;
  AST_ID FUNCTION_BODY = 5;
  AST_ID CALL_NAME = 6;
  AST_ID CALL_ARGS = 7;
  AST_ID VAR_NAME = 8;
  AST_ID VAR_TYPE = 9;
  AST_ID VAR_INITVAL = 10;

  #undef AST_ID
} // namespace ASTNodeID


struct AST {
  using ValueType = std::variant<
    std::nullptr_t,
    int64_t,
    double,
    char,
    std::string,
    AST,
    std::vector<AST>
  >;

  ASTType type;
  std::unordered_map<uint32_t, ValueType> values;

  inline ValueType& operator[](uint32_t id) {
    return values[id];
  }

  inline const ValueType& at(uint32_t id) const {
    return values.at(id);
  }

  friend inline bool operator ==(const AST &lhs, const AST &rhs) {
    return lhs.type == rhs.type;
  }
  friend inline bool operator ==(const AST &lhs, const ASTType &rhs) {
    return lhs.type == rhs;
  }
  inline bool operator !() const {
    return type == NONE;
  }
};

class Parser {
private:
  Lexer &m_lexer;

public:
  Parser(Lexer &lexer);

private:
  Token isTokenKeyword(const std::string &value);
  Token isTokenOperator(const std::string &value);
  Token isTokenPunctuator(const std::string &value);

  AST parseToplevel();
};