#pragma once
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
	CLI::App app{ "Wasp Interpreter" };

	// RUN Command

	CLI::App* run_cmd = app.add_subcommand("run", "Executes a wasp file");

	string filepath;
	run_cmd
		->add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	CLI11_PARSE(app, argc, argv);

	string raw_source = read_source(filepath);

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	Parser parser(tokens);
	Module mod = parser.execute();

	Interpreter interpreter(mod);
	interpreter.execute();
}