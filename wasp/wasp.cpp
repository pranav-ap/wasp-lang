#pragma once
#include "logger.h"
#include "CLI11.hpp"

#include "file_io.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#include <vector>
#include <string>

using std::string;
using std::vector;

int main(int argc, char** argv)
{
	TITLE("\n [ Wasp Interpreter ] ");
	TITLE("Inefficient, Elegant and Fun!");

	CLI::App app{ "Wasp Interpreter" };

	string filepath;
	app.add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	CLI11_PARSE(app, argc, argv);

	string raw_source = read_source(filepath);

	INFO("Executing Lexer..");
	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();
	INFO("Done");

	INFO("Executing Parser..");
	Parser parser(tokens);
	Module mod = parser.execute();
	INFO("Done");

	INFO("Interpreting code..");
	Interpreter interpreter(mod);
	interpreter.execute();
	INFO("Done");
}