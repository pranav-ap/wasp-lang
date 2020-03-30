#pragma once
#include <iostream>
#include <vector>

#include "io.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

using namespace std;

int main()
{
	cout << "\n Wasp Language Interpreter \n" << endl;

	string raw_source = read_source("./../scraps.wasp");

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	Parser parser(tokens);
	Module mod = parser.execute();

	Interpreter interpreter(mod);
	interpreter.execute();
}