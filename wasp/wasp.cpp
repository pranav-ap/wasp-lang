#pragma once
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "Statement.h"
#include "ASTVisualizer.h"
#include "SemanticAnalyzer.h"
#include "Compiler.h"
#include "InstructionPrinter.h"

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
	vector<Token_ptr> tokens = lexer->execute(raw_source);

	Parser_ptr parser = new Parser();
	File_ptr ast = parser->execute(tokens);

	ASTVisualizer_ptr vis = make_unique<ASTVisualizer>();
	vis->generate_dot_file(ast);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->execute(ast);

	Compiler_ptr compiler = make_unique<Compiler>();
	ByteVector instructions = compiler->execute(ast);


	return 0;
}