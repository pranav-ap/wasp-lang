#pragma once
#include "CLI11.hpp"

#include "file_io.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#include <vector>
#include <string>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

void run(string filepath, bool is_verbose)
{
	string raw_source = read_source(filepath);

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	Parser parser(tokens);
	Module mod = parser.execute();

	Interpreter interpreter(mod);
	interpreter.execute();
}

void process_command(bool version_request, string filepath, bool is_verbose)
{
	if (version_request)
	{
		cout << "wasp version 0.1" << endl;
		return;
	}

	run(filepath, is_verbose);
}

int main(int argc, char** argv)
{
	CLI::App app{ "Wasp Interpreter" };

	// Version Flag

	bool version_request = false;
	app
		.add_flag("-v,--version", version_request, "Prints the version of wasp")
		->ignore_case();;

	// RUN Command

	CLI::App* run_cmd = app.add_subcommand("run", "Executes a wasp file");

	string filepath;
	run_cmd
		->add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	bool is_verbose = false;
	run_cmd
		->add_flag("-t, --trace", is_verbose, "Provides interpreter traces")
		->ignore_case();

	CLI11_PARSE(app, argc, argv);

	process_command(version_request, filepath, is_verbose);
}