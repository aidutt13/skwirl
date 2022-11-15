#include "Chars.h"

using namespace std::literals;

bool strchr(const char *str, char c) {
  const char *s = str;
  while (*s != '\0') {
    if (c == *s)
      return true;
    
    ++s;
  }
  return false;
}

bool isWhitespace(const char *c) {
  return *c == ' ' || *c == '\t' || *c == '\r' || *c == '\v' || *c == '\f';
}

bool isIdentifier(const char *c) {
  return (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || *c == '_';
}

std::string KEYWORDS[] = {
  "let"s, "as"s, "const"s, "define"s, "begin"s, "do"s, "end"s, "if"s, "then"s, "else"s, ""s
};
bool isKeyword(const char *c) {
  std::string *keywords = KEYWORDS;
  while (!keywords->empty()) {
    if (*keywords == c) {
      return true;
    }
    keywords++;
  }
  return false;
}

bool isDigit(const char *c) {
  return *c >= '0' && *c <= '9';
}

bool isStringQuote(const char *c) {
  return *c == '"';
}

bool isCharQuote(const char *c) {
  return *c == '\'';
}

const char *OPERATORS = "+-*/%&|^~!?:=<>";
bool isOperator(const char *c) {
  return strchr(OPERATORS, *c);
}

const char *PUNCTUATORS = ".,;()[]{}\n";
bool isPunctuator(const char *c) {
  return strchr(PUNCTUATORS, *c);
}