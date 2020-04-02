#pragma once
#define ELPP_STL_LOGGING

#include "easylogging++.h"
#include "CLI11.hpp"

#include "io.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#include <vector>
#include <string>

INITIALIZE_EASYLOGGINGPP

using std::string;
using std::vector;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
	CLI::App app{ "Wasp Language Interpreter" };

	el::Configurations conf("./../log-config.conf");
	el::Loggers::reconfigureAllLoggers(conf);

	LOG(INFO) << "Wasp Language Interpreter";

	string filepath;
	app.add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	CLI11_PARSE(app, argc, argv);

	string raw_source = read_source(filepath);

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	LOG(INFO) << tokens;

	Parser parser(tokens);
	Module mod = parser.execute();

	Interpreter interpreter(mod);
	interpreter.execute();
}