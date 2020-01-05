#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include "Lexer.h"
#include "Parser.h"

using namespace std;

string read_source(string path) {
	ifstream infile(path);
	string line;
	string raw_source;

	while (getline(infile, line))
	{
		raw_source.append(line);
		raw_source.append("\n");
	}

	return raw_source;
}

int main()
{
	cout << "\n Wasp Language Interpreter \n" << endl;

	string raw_source = read_source("main.wasp");

	Lexer lexer(raw_source);
	vector<shared_ptr<Token>> tokens = lexer.execute();

	cout << "\n Scanned Tokens \n" << endl;

	for (auto const& token : tokens)
	{
		token->print();
	}

	Parser parser(tokens);
	Module mod = parser.execute();
	mod.print();
}