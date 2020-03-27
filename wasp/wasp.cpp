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

	cout << "\n Scanned Tokens \n" << endl;

	for (auto const& token : tokens)
		token->print();

	Parser parser(tokens);
	Module mod = parser.execute();
	mod.print();

	Interpreter interpreter(mod);
	interpreter.execute();
}