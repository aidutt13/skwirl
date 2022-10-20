#include "Lexer.h"

const std::unordered_map<TokenType, std::string> Token::typeNames = {
  {NONE, "NONE"},
  {EOB, "EOB"},
  {IDENTIFIER, "IDENTIFIER"},
  {KEYWORD, "KEYWORD"},
  {INTEGER, "INTEGER"},
  {FLOAT, "FLOAT"},
  {STRING, "STRING"},
  {CHAR, "CHAR"},
  {OPERATOR, "OPERATOR"},
  {PUNCTUATOR, "PUNCTUATOR"},
};

UnexpectedCharacterException::UnexpectedCharacterException(char c, uint32_t row, uint32_t col) {
  std::stringstream ss;
  ss << "Unexpected character '" << c << "' @ (" << row << ", " << col << ")";
  m_message = ss.str();
}

Lexer::Lexer(std::basic_istream<char> &stream) {
  m_data.clear();
  m_data << stream.rdbuf();
  m_currentToken = Token{ NONE, "", 0, 0 };
}

char Lexer::peekChar() {
  return m_data.peek();
}

char Lexer::nextChar() {
  char c = m_data.get();
  if (c == '\n') {
    m_row++;
    m_lastLineCol = m_col;
    m_col = 0;
  } else {
    m_col++;
  }
  return c;
}

void Lexer::putBackChar() {
  m_data.unget();
  if (m_col == 0) {
    m_row--;
    m_col = m_lastLineCol;
  } else {
    m_col--;
  }
}

bool Lexer::eof() {
  return peekChar() == EOF;
}

void Lexer::skipWhitespace() {
  readWhile(&isWhitespace);
}

void Lexer::skipComment() {
  readWhile([](const char *c) { return *c != '\n'; });
}

std::string Lexer::readWhile(std::function<bool(const char *)> predicate) {
  std::string result;
  char cs[2];
  cs[0] = peekChar();
  cs[1] = '\0';
   
  while (!eof() && predicate(cs)) {
    result += nextChar();
    cs[0] = peekChar();
  }
  return result;
}

std::unordered_map<char, char> escapeMap = {
  { 'a', '\a' },
  { 'b', '\b' },
  { 'f', '\f' },
  { 'n', '\n' },
  { 'r', '\r' },
  { 't', '\t' },
  { 'v', '\v' },
  { '\\', '\\' },
  { '\'', '\'' },
  { '\"', '\"' },
  { '?', '\?' },
};
std::string Lexer::readEscaped(char end) {
  std::string result;
  char cs[2];
  cs[1] = '\0';

  bool esc = false;

  while (!eof()) {
    cs[0] = nextChar();
    if (esc) {
      if (escapeMap.find(cs[0]) != escapeMap.end()) {
        result += escapeMap[cs[0]];
      } else {
        result += cs[0];
      }
      esc = false;
    } else if (cs[0] == '\\') {
      esc = true;
    } else if (cs[0] == end) {
      break;
    } else {
      result += cs[0];
    }
  }

  return result;
}

Token Lexer::readNextToken() {
  skipWhitespace();
  if (eof()) {
    return Token{ EOB, "", m_row, m_col };
  }

  if (peekChar() == '/') {
    nextChar();
    if (peekChar() == '/') {
      skipComment();
      return readNextToken();
    } else {
      putBackChar();
    }
  }

  char cs[2];
  cs[0] = peekChar();
  cs[1] = '\0';

  if (isDigit(cs)) {
    return readNumberToken();
  }
  if (isIdentifier(cs)) {
    return readIdentifierToken();
  }
  if (isStringQuote(cs)) {
    auto row = m_row, col = m_col;
    nextChar();
    return Token{ STRING, readEscaped('"'), row, col };
  }
  if (isCharQuote(cs)) {
    auto row = m_row, col = m_col;
    nextChar();
    return Token{ CHAR, readEscaped('\''), row, col };
  }
  if (isOperator(cs)) {
    auto row = m_row, col = m_col;
    return Token{ OPERATOR, readWhile(&isOperator), row, col };
  }
  if (isPunctuator(cs)) {
    auto row = m_row, col = m_col;
    return Token{ PUNCTUATOR, std::string(1, nextChar()), row, col };
  }
  
  throw UnexpectedCharacterException(*cs, m_row, m_col);
}

Token Lexer::readNumberToken() {
  auto row = m_row, col = m_col;
  bool dot = false;
  std::string value = readWhile([&dot](const char *c) {
    if (*c == '.') {
      if (dot) {
        return false;
      }
      dot = true;
      return true;
    }
    return isDigit(c);
  });

  return Token{ dot ? FLOAT : INTEGER, value, row, col };
}

Token Lexer::readIdentifierToken() {
  auto row = m_row, col = m_col;
  std::string value = readWhile(&isIdentifier);

  if (isKeyword(value.c_str())) {
    return Token{ KEYWORD, value, row, col };
  }

  return Token{ IDENTIFIER, value, row, col };
}

Token Lexer::nextToken() {
  m_currentToken = readNextToken();
  return m_currentToken;
}

Token Lexer::currentToken() {
  return m_currentToken;
}