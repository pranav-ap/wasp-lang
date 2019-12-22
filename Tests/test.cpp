#include "pch.h"

#include <string>

#include "../wasp-lang/Lexer.h"

using namespace std;

TEST(LexerTests, TestName) {
  const string code = R"(
let a : num = 54;
)";

  EXPECT_EQ(code, "hi bye");
}