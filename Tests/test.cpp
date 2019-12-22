#include "pch.h"

#include <string>

#include "../wasp-lang/Lexer.h"

using namespace std;

TEST(LexerTests, LetDeclaration) {
  const string raw_source = "let a : str = \"hello world!\"";

  vector<TokenType> expected_tokens = {
    Keyword(KeywordType::LET, 1, 4),
    Identifier("a", false, 1, 6),
    Punctuation(PunctuationType::COLON, 1, 8),
    Keyword(KeywordType::STR, 1, 12),
    Punctuation(PunctuationType::EQUAL, 1, 14),
    StringLiteral("hello world!", 1, 29)
  };

  Lexer lexer(raw_source);
  vector<unique_ptr<TokenType>> tokens = lexer.execute();

  for (auto const& token : tokens)
  {
	  auto g = token.get();

	  if (typeid(Identifier) == typeid(g)) {
		  auto f = static_cast<Identifier*>(g);
	  }
  }
}