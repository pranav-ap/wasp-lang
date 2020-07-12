#pragma once

#include "WaspCLI.h"
#include "spdlog.h"
#include "file_io.h"
#include "Lexer.h"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std::chrono;
using std::string;
using std::wstring;
using std::vector;
using std::cout;
using std::endl;

WaspCLI::WaspCLI()
{
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
		->ignore_case();

	// RUN Command

	CLI::App* run_cmd = app.add_subcommand("run", "Executes a wasp file");

	run_cmd
		->add_option("-f, --file", filepath, "Provide the path to your main file")
		->required()
		->check(CLI::ExistingFile);

	run_cmd->callback([&]() { run(); });
}

CLI::App& WaspCLI::get_app()
{
	return app;
}

void WaspCLI::run()
{
	//spdlog::info("Welcome to spdlog!");

	auto start = high_resolution_clock::now();

	wstring raw_source = read_source(filepath);

	Lexer lexer;
	vector<Token_ptr> tokens = lexer.execute(raw_source);

	auto end = high_resolution_clock::now();

	// Calculate time taken

	auto elapsed_seconds = end - start;
	double time_taken = elapsed_seconds.count();

	cout << "Time Taken : " << time_taken << "seconds";

	//spdlog::info("Time Taken : {} seconds", time_taken);
}