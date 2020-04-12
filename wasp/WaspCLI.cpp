#include "WaspCLI.h"
#include "file_io.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <string>

using std::string;
using std::vector;
using std::cout;
using std::endl;

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

	// REPL Command

	CLI::App* repl_cmd = app.add_subcommand("repl", "Starts a REPL session");

	repl_cmd
		->add_flag("-t, --trace", provide_trace, "Provides interpreter traces")
		->ignore_case();

	repl_cmd->callback([&]() { open_repl(); });
}

CLI::App& WaspCLI::get_app()
{
	return app;
}

void WaspCLI::run()
{
	string raw_source = read_source(filepath);

	Lexer lexer(raw_source);
	vector<Token_ptr> tokens = lexer.execute();

	Parser parser(tokens);
	Module mod = parser.execute();

	auto env = std::make_shared<Environment>();
	env->enter_module_scope();

	Interpreter interpreter(move(env));
	interpreter.execute(mod);
}

void WaspCLI::open_repl()
{
	auto env = std::make_shared<Environment>();
	env->enter_module_scope();

	Interpreter interpreter(move(env));

	// LOOP

	cout << "\n Welcome to the Wasp REPL! \n" << endl;

	while (true)
	{
		// READ

		cout << ">> ";

		std::string raw_source;
		std::string line;

		while (true)
		{
			std::getline(std::cin, line);

			if (line.empty()) {
				break;
			}

			raw_source.append(line);
			raw_source.append("\n");
		}

		if (line == ".exit")
		{
			return;
		}

		// EVAL

		Lexer lexer(raw_source);
		vector<Token_ptr> tokens = lexer.execute();

		Parser parser(tokens);
		Module mod = parser.execute();

		interpreter.execute(mod);
	}
}