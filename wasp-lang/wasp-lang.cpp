#pragma once
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "Statement.h"
#include "SemanticAnalyzer.h"
#include "MemorySystem.h"
#include "Compiler.h"
//#include "CFGBuilder.h"
#include <string>
#include <iostream>
#include <memory>
#include <vector>

using std::make_unique;
using std::make_shared;
using std::cout;
using std::endl;
using std::vector;
using std::wstring;

int main()
{
	wstring raw_source = read_source("../examples/main.txt");

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->execute(raw_source);

	Parser_ptr parser = make_unique<Parser>();
	Module_ptr ast = parser->execute(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->execute(ast);

	MemorySystem_ptr memory = make_shared<MemorySystem>();

	Compiler_ptr compiler = make_unique<Compiler>(memory);
	compiler->execute(ast);

	//CFGBuilder_ptr cfg_builder = make_unique<CFGBuilder>(memory);
	//CFG_ptr cfg = cfg_builder->execute();

	return 0;
}