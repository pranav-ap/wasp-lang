#pragma once
#include "WaspCLI.h"
#include "file_io.h"
#include "Lexer.h"
#include "Parser.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using namespace std::chrono;

WaspCLI::WaspCLI()
{
	provide_trace = false;
	filepath = "";

	// Setup CLI options and commands

	app.name("Wasp CLI");
	app.description("Interpreter for Wasp Lang");
	app.get_formatter()->column_width(50);

	app
		.add_flag(
			"-v,--version",
			[](int) { cout << "wasp version 0.1" << endl; },
			"Prints the version of wasp"
		)
		->ignore_case();;

	// RUN Command

	CLI::App* run_cmd = app.add_subcommand("run", "Executes a wasp file");

	run_cmd
		->add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	run_cmd
		->add_flag("-t, --trace", provide_trace, "Provides interpreter traces")
		->ignore_case();

	run_cmd->callback([&]() { run(); });
}

CLI::App& WaspCLI::get_app()
{
	return app;
}

void WaspCLI::run()
{
	auto start = high_resolution_clock::now();

	string raw_source = read_source(filepath);

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	Parser parser(tokens);
	Module mod = parser.execute();

	auto end = high_resolution_clock::now();
	duration<double> elapsed_seconds = end - start;

	double time_taken = elapsed_seconds.count();
	cout << " Time Taken : " << time_taken << " seconds" << endl;
}