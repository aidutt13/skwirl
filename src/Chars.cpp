#include "Chars.h"

bool isWhitespace(const char *c) {
  return *c == ' ' || *c == '\t' || *c == '\r' || *c == '\n' || *c == '\v' || *c == '\f';
}

bool isIdentifier(const char *c) {
  return (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || (*c >= '0' && *c <= '9') || *c == '_';
}

const char *KEYWORDS[] = {
  "let", "as", "const", "define", "begin", "do", "end", "if", "then", "else"
};
bool isKeyword(const char *c) {
  const char** keywords = KEYWORDS;
  while (*keywords) {
    if (strcmp(*keywords, c) == 0) {
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
  return strchr(OPERATORS, *c) != NULL;
}

const char *PUNCTUATORS = ".,;()[]{}";
bool isPunctuator(const char *c) {
  return strchr(PUNCTUATORS, *c) != NULL;
}