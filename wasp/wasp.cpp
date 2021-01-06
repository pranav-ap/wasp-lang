#pragma once
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "Statement.h"
#include "SemanticAnalyzer.h"
#include "Compiler.h"
#include "ObjectStore.h"
#include "CodeObject.h"
#include "VirtualMachine.h"

#include <string>
#include <memory>
#include <vector>

using std::make_unique;
using std::make_shared;
using std::vector;
using std::wstring;

int main()
{
	wstring raw_source = read_source("../examples/main.wasp");

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->run(raw_source);

	Parser_ptr parser = new Parser();
	Module_ptr ast = parser->run(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->run(ast);

	Compiler_ptr compiler = make_unique<Compiler>();
	auto [object_store, code_object] = compiler->run(ast);

	//VirtualMachine_ptr vm = make_unique<VirtualMachine>(object_store, code_object);
	//vm->run();

	return 0;
}