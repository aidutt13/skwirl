#pragma once

#include "Lexer.h"
#include <memory>
#include <variant>
#include <vector>

enum class ASTType {
  NONE,
  EOB,
  NAME,
  BOOL,
  INTEGER,
  FLOAT,
  STRING,
  CHAR,

  PROG,
  FUNCTION,
  CALL,
  VAR,
  BINARY,
  ASSIGN,
  IF,
};

namespace astid {
  #define AST_ID constexpr uint32_t
  
  AST_ID VALUE = 10;
  AST_ID PROG = 20;
  AST_ID FUNCTION_NAME = 30;
  AST_ID FUNCTION_PARAMS = 31;
  AST_ID FUNCTION_BODY = 32;
  AST_ID FUNCTION_RETTYPE = 33;
  AST_ID CALL_FUNC = 40;
  AST_ID CALL_ARGS = 41;
  AST_ID VAR_NAME = 50;
  AST_ID VAR_TYPE = 51;
  AST_ID VAR_INITVAL = 52;
  AST_ID BINARY_LEFT = 60;
  AST_ID BINARY_RIGHT = 61;
  AST_ID BINARY_OP = 62;
  AST_ID IF_COND = 70;
  AST_ID IF_THEN = 71;
  AST_ID IF_ELSE = 72;

  #undef AST_ID
} // namespace ASTNodeID

extern const std::unordered_map<std::string, uint32_t> OPERATOR_PRECEDENCE;

struct AST {
  using Array = std::vector<AST>;
  using Ptr = std::shared_ptr<AST>;
  using Function = std::function<AST()>;

  using ValueType = std::variant<
    std::nullptr_t,
    bool,
    int64_t,
    double,
    char,
    std::string,
    AST::Ptr,
    AST::Array
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
    return type == ASTType::NONE;
  }

  friend std::ostream &operator <<(std::ostream &os, const AST &ast) {
    static const std::unordered_map<ASTType, std::string> AST_TYPE_NAMES = {
      { ASTType::NONE, "NONE" },
      { ASTType::EOB, "EOB" },
      { ASTType::NAME, "NAME" },
      { ASTType::BOOL, "BOOL" },
      { ASTType::INTEGER, "INTEGER" },
      { ASTType::FLOAT, "FLOAT" },
      { ASTType::STRING, "STRING" },
      { ASTType::CHAR, "CHAR" },
      { ASTType::PROG, "PROG" },
      { ASTType::FUNCTION, "FUNCTION" },
      { ASTType::CALL, "CALL" },
      { ASTType::VAR, "VAR" },
      { ASTType::BINARY, "BINARY" },
      { ASTType::ASSIGN, "ASSIGN" },
      { ASTType::IF, "IF" },
    };
    os << AST_TYPE_NAMES.at(ast.type);
    return os;
  }
};

class Parser {
private:
  Lexer &m_lexer;

public:
  Parser(Lexer &lexer);

  AST operator ()();

private:
  Token isTokenKeyword(const std::string &value);
  Token isTokenOperator(const std::string &value);
  Token isTokenPunctuator(const std::string &value);

  void skipKeyword(const std::string &value);
  void skipOperator(const std::string &value);
  void skipPunctuator(const std::string &value);

  template<typename T>
  AST::Array delimited(const std::string &start, const std::string &stop, const std::string &separator, std::function<T()> parser) {
    std::vector<T> a;
    bool first = true;
    skipPunctuator(start);
    while (m_lexer.currentToken() != TokenType::EOB) {
      if (!!isTokenPunctuator(stop)) {
        break;
      }
      if (first) {
        first = false;
      } else {
        skipPunctuator(separator);
      }
      if (!!isTokenPunctuator(stop)) {
        break;
      }
      a.push_back(parser());
    }
    skipPunctuator(stop);

    return a;
  }

  AST parseToplevel();
  AST parseAtom();
  AST parseExpression();
  AST parseCall(AST function);
  AST parseProg();
  AST parseIf();
  AST parseBool();
  AST parseFunction();
  AST parseVar();

  AST maybeCall(AST::Function expr);
  AST maybeBinary(AST expr, uint32_t prec);
};