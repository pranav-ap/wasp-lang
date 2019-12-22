#include "pch.h"
#include <string>
#include "../wasp-lang/Lexer.h"

using namespace std;

TEST(LexerTests, LetDeclaration) {
  const string raw_source = "let a : str = \"hello world!\"";

  vector<Token> expected_tokens = {
    Token(TokenType::LET, "let", 1, 4),
    Token(TokenType::Identifier, "a", 1, 6),
    Token(TokenType::COLON, ":", 1, 8),
    Token(TokenType::STR, "str", 1, 12),
    Token(TokenType::EQUAL, "=", 1, 14),
    Token(TokenType::StringLiteral, "hello world!", 1, 29),
    Token(TokenType::EOL, "\\n", 1, 30)
  };

  Lexer lexer(raw_source);
  vector<unique_ptr<Token>> tokens = lexer.execute();

  for (auto const& token : tokens)
  {
	 auto g = token.get();
  }
}