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

void prepare_natives(BuiltinsManager_ptr builtins_manager)
{
	wstring raw_source = read_source("../examples/natives.wasp");

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->run(raw_source);

	Parser_ptr parser = new Parser();
	Module_ptr ast = parser->run(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>(builtins_manager);
	semantic_analyser->run(ast);
}

int main()
{
	BuiltinsManager_ptr builtins_manager = make_shared<BuiltinsManager>();
	prepare_natives(builtins_manager);

	wstring raw_source = read_source("../examples/main.wasp");

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->run(raw_source);

	Parser_ptr parser = new Parser();
	Module_ptr ast = parser->run(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>(builtins_manager);
	semantic_analyser->run(ast);

	Compiler_ptr compiler = make_unique<Compiler>();
	auto [constant_pool, code_object] = compiler->run(ast);

	VirtualMachine_ptr vm = make_unique<VirtualMachine>(constant_pool, code_object);
	vm->run();

	return 0;
}