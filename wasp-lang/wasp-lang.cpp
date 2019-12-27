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
	cout << " Wasp Language Interpreter " << endl;

	string raw_source = read_source("lexer_test.wasp");
	cout << raw_source << endl;

	Lexer lexer(raw_source);
	vector<shared_ptr<Token>> tokens = lexer.execute();

	for (auto const& token : tokens)
	{
		token->print();
	}

	Parser parser(tokens);
	Module mod = parser.execute();
}