#include <iostream>
#include <fstream>

#include "Lexer.h"

int main() {

  std::ifstream file("./test.txt");

  Lexer lexer(file);

  while (!lexer.eof()) {
    std::cout << lexer.nextToken() << '\n';
  }

  return 0;
}