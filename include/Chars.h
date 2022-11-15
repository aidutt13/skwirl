#pragma once

#include <string>

bool isWhitespace(const char *c);
bool isIdentifier(const char *c);
bool isKeyword(const char *c);
bool isDigit(const char *c);
bool isStringQuote(const char *c);
bool isCharQuote(const char *c);
bool isOperator(const char *c);
bool isPunctuator(const char *c);
