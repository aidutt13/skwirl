#include "Parser.h"

Token Parser::isTokenKeyword(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::KEYWORD && value.size() && tok.value == value) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

Token Parser::isTokenOperator(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::OPERATOR && value.size() && tok.value == value) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

Token Parser::isTokenPunctuator(const std::string &value) {
  auto tok = m_lexer.currentToken();
  if (tok == TokenType::PUNCTUATOR && value.size() && tok.value == value) {
    return tok;
  }
  return Token{TokenType::NONE, "", tok.row, tok.col};
}

AST Parser::parseToplevel() {
  AST ast;
  ast.type = ASTType::PROG;
  ast[astid::PROG] = std::vector<AST>{};
  while (m_lexer.currentToken() != TokenType::EOB) {
    ast
  }
  return ast;
}