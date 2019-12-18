#include <iostream>
#include <fstream>
#include <memory>
#include "Lexer.h"

using namespace std;

string read_source() {
	ifstream infile("main.wsp");
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

	string raw_source = read_source();
	cout << raw_source << endl;

	Lexer lexer(raw_source);
	vector<unique_ptr<Token>> tokens = lexer.execute();

	for (auto const& token : tokens)
	{
		token->print();
	}
}