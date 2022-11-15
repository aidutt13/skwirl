#include "Parser.h"

#include <iostream>
using std::cout, std::endl;

const std::unordered_map<std::string, uint32_t> OPERATOR_PRECEDENCE = {
  {"=", 1},
  {"<", 7}, {">", 7}, {"<=", 7}, {">=", 7}, {"==", 7}, {"!=", 7},
  {"+", 10}, {"-", 10},
  {"*", 20}, {"/", 20}, {"%", 20}, 
};

Parser::Parser(Lexer &lexer) : m_lexer(lexer) { }

AST Parser::operator()() {
  return parseToplevel();
}

std::string escapeChar(char c) {
  for (auto kv : escapeMap) {
    if (kv.second == c) {
      return "\\" + std::string(1, kv.first);
    }
  }
  return std::string(1, c);
}

Token Parser::isTokenKeyword(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::KEYWORD && (value.size() == 0 || tok.value == value)) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

Token Parser::isTokenOperator(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::OPERATOR && (value.size() == 0 || tok.value == value)) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

Token Parser::isTokenPunctuator(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::PUNCTUATOR && (value.size() == 0 || tok.value == value)) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

void Parser::skipKeyword(const std::string &value) {
  if (!!isTokenKeyword(value)) {
    m_lexer.nextToken();
  }
  else {
    throw std::runtime_error("Expected keyword '" + value + "' at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }
}

void Parser::skipOperator(const std::string &value) {
  if (!!isTokenOperator(value)) {
    m_lexer.nextToken();
  }
  else {
    throw std::runtime_error("Expected operator '" + value + "' at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }
}

void Parser::skipPunctuator(const std::string &value) {
  cout << "skipPunctuator(" << escapeChar(value[0]) << "); currentToken: " << m_lexer.currentToken() << endl;
  if (!!isTokenPunctuator(value)) {
    m_lexer.nextToken();
  }
  else {
    throw std::runtime_error("Expected punctuator '" + escapeChar(value[0]) + "' at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }
}

AST Parser::parseToplevel() {
  AST ast;
  ast.type = ASTType::PROG;
  ast[astid::PROG] = std::vector<AST>{};
  while (m_lexer.currentToken() != TokenType::EOB) {
    cout << "Parsing: " << m_lexer.currentToken() << endl;
    std::get<std::vector<AST>>
      (ast[astid::PROG]).push_back(parseExpression());
    skipPunctuator("\n");
  }
  return ast;
}

AST Parser::parseAtom() {
  return maybeCall([this]() -> AST {
Parser_parseAtomStart:
    if (!!this->isTokenPunctuator("\n")) {
      this->m_lexer.nextToken();
      goto Parser_parseAtomStart;
    }

    if (!!this->isTokenPunctuator("(")) {
      cout << "Parsing: " << m_lexer.currentToken() << endl;
      this->m_lexer.nextToken();
      cout << "Parsing: " << m_lexer.currentToken() << endl;
      auto exp = this->parseExpression();
      cout << "Parsing: " << m_lexer.currentToken() << endl;
      this->skipPunctuator(")");
      return exp;
    }

    if ( false
      || !!this->isTokenKeyword("begin")
      || !!this->isTokenKeyword("do")
      || !!this->isTokenKeyword("then")
    ) {
      this->m_lexer.nextToken();
      return this->parseProg();
    }

    if (!!this->isTokenKeyword("if")) {
      return this->parseIf();
    }

    if (!!this->isTokenKeyword("true") || !!this->isTokenKeyword("false")) {
      return this->parseBool();
    }

    if (!!this->isTokenKeyword("define")) {
      return this->parseFunction();
    }

    if (!!this->isTokenKeyword("let")) {
      this->m_lexer.nextToken();
      return this->parseVar();
    }

    auto tok = this->m_lexer.nextToken();
    AST ast;

    if (tok == TokenType::IDENTIFIER) {
      ast.type = ASTType::NAME;
      ast[astid::VALUE] = tok.value;
      return ast;
    }

    if (tok == TokenType::INTEGER) {
      ast.type = ASTType::INTEGER;
      ast[astid::VALUE] = std::stol(tok.value);
      return ast;
    }

    if (tok == TokenType::FLOAT) {
      ast.type = ASTType::FLOAT;
      ast[astid::VALUE] = std::stod(tok.value);
      return ast;
    }

    if (tok == TokenType::STRING) {
      ast.type = ASTType::STRING;
      ast[astid::VALUE] = tok.value;
      return ast;
    }

    if (tok == TokenType::CHAR) {
      ast.type = ASTType::CHAR;
      ast[astid::VALUE] = tok.value[0];
      return ast;
    }

    throw std::runtime_error("Unexpected token '" + tok.value + "' at " + std::to_string(tok.row) + ":" + std::to_string(tok.col));
  });
}

AST Parser::parseExpression() {
  return maybeCall([this]() {
    return this->maybeBinary(this->parseAtom(), 0);
  });
}

AST Parser::parseCall(AST function) {
  AST ast;
  ast.type = ASTType::CALL;
  ast[astid::CALL_FUNC] = std::make_shared<AST>(std::move(function));
  ast[astid::CALL_ARGS] = delimited<AST>("(", ")", ",", [this]() {
    return this->parseExpression();
  });

  return ast;
}

AST Parser::parseProg() {
  std::vector<AST> prog;
  while (!isTokenKeyword("end")) {
    if (m_lexer.eof()) {
      throw std::runtime_error("Expected 'end' at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
    }
    prog.push_back(parseExpression());
    
    skipPunctuator("\n");
  }
  m_lexer.nextToken();
  AST ast;
  ast.type = ASTType::PROG;
  ast[astid::PROG] = std::move(prog);

  return ast;
}

AST Parser::parseIf() {
  m_lexer.nextToken();
  auto cond = parseExpression();
  auto then = parseExpression();

  AST::ValueType else_ = nullptr;
  if (!!isTokenKeyword("else")) {
    m_lexer.nextToken();
    else_ = std::make_shared<AST>(parseExpression());
  }

  AST ast;
  ast.type = ASTType::IF;
  ast[astid::IF_COND] = std::make_shared<AST>(std::move(cond));
  ast[astid::IF_THEN] = std::make_shared<AST>(std::move(then));
  ast[astid::IF_ELSE] = std::move(else_);

  return ast;
}

AST Parser::parseBool() {
  AST ast;
  ast.type = ASTType::BOOL;
  ast[astid::VALUE] = !!isTokenKeyword("true");
  m_lexer.nextToken();
  return ast;
}

AST Parser::parseFunction() {
  AST ast;

  m_lexer.nextToken();

  auto name = m_lexer.nextToken();
  cout << "  Name: " << name << endl;

  if (name != TokenType::IDENTIFIER) {
    throw std::runtime_error("Expected identifier at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }

  ast.type = ASTType::FUNCTION;
  ast[astid::FUNCTION_NAME] = name.value;
  ast[astid::FUNCTION_PARAMS] = delimited<AST>("(", ")", ",", [this]() { return this->parseVar(); });

  skipKeyword("as");
  auto type = m_lexer.nextToken(); // TODO: parse types

  ast[astid::FUNCTION_BODY] = std::make_shared<AST>(parseExpression());
  ast[astid::FUNCTION_RETTYPE] = type.value;

  if (std::get<AST::Ptr>(ast[astid::FUNCTION_BODY])->type != ASTType::PROG) {
    throw std::runtime_error("Expected function body to be a program at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }

  return ast;
}

AST Parser::parseVar() {
  auto name = m_lexer.nextToken();
  if (name != TokenType::IDENTIFIER) {
    throw std::runtime_error("Expected identifier at " + std::to_string(m_lexer.currentToken().row) + ":" + std::to_string(m_lexer.currentToken().col));
  }

  skipKeyword("as");

  auto type = m_lexer.nextToken(); // TODO: parse type

  AST::ValueType value = nullptr;

  if (!!isTokenOperator("=")) {
    m_lexer.nextToken();
    value = std::make_shared<AST>(parseExpression());
  }

  AST ast;
  ast.type = ASTType::VAR;
  ast[astid::VAR_NAME] = std::move(name.value);
  ast[astid::VAR_TYPE] = std::move(type.value);
  ast[astid::VAR_INITVAL] = std::move(value);

  return ast;
}

AST Parser::maybeCall(AST::Function expr) {
  auto e = expr();
  return !!isTokenPunctuator("(") ? parseCall(e) : e;
}

AST Parser::maybeBinary(AST left, uint32_t myPrec) {
  auto tok = isTokenOperator("");
  if (!tok) {
    return left;
  }

  auto hisPrec = OPERATOR_PRECEDENCE.at(tok.value);
  if (hisPrec < myPrec) {
    return left;
  }

  m_lexer.nextToken();
  auto right = maybeBinary(parseAtom(), hisPrec);
  AST binary;
  binary.type = tok.value == "=" ? ASTType::ASSIGN : ASTType::BINARY;
  binary[astid::BINARY_OP] = std::move(tok.value);
  binary[astid::BINARY_LEFT] = std::make_shared<AST>(std::move(left));
  binary[astid::BINARY_RIGHT] = std::make_shared<AST>(std::move(right));

  return maybeBinary(std::move(binary), myPrec);
}