#include <iostream>
#include <fstream>

#include "Parser.h"

int main() {

  std::ifstream file("./test.txt");

  Lexer lexer(file);
  Parser parser(lexer);

  // while (lexer.currentToken() != TokenType::EOB) {
  //   std::cout << lexer.nextToken() << std::endl;
  // }

  AST ast = parser();
  std::cout << ast << std::endl;


  return 0;
}