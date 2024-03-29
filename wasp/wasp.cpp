#pragma once
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "Statement.h"
#include "SemanticAnalyzer.h"
#include "Builtins.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include "Compiler.h"
#include "VirtualMachine.h"
#include "Assertion.h"
#include <string>
#include <memory>
#include <vector>

using std::make_unique;
using std::make_shared;
using std::vector;
using std::wstring;
using std::string;

Module_ptr parse(string path)
{
	wstring raw_source = read_source(path);

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->run(raw_source);

	Parser_ptr parser = new Parser();
	Module_ptr ast = parser->run(tokens);

	return ast;
}

int main()
{
	BuiltinsManager_ptr builtins_manager = make_shared<BuiltinsManager>();
	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>(builtins_manager);

	// Analyse native code

	Module_ptr ast = parse("../examples/natives.wasp");
	semantic_analyser->run(ast);

	// Analyse user code

	ast = parse("../examples/main.wasp");
	semantic_analyser->run(ast);

	// Compile 

	Compiler_ptr compiler = make_unique<Compiler>();
	auto [constant_pool, code_object] = compiler->run(ast);

	// Run

	VirtualMachine_ptr vm = make_unique<VirtualMachine>(constant_pool, code_object);
	vm->run();

	return 0;
}