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
using std::string;
using std::wstring;

ByteVector build(string filename)
{
	wstring raw_source = read_source(filename);

	Lexer_ptr lexer = make_unique<Lexer>();
	vector<Token_ptr> tokens = lexer->execute(raw_source);

	Parser_ptr parser = new Parser();
	Module_ptr ast = parser->execute(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->execute(ast);

	Compiler_ptr compiler = make_unique<Compiler>();
	ByteVector instructions = compiler->execute(ast);

	return instructions;
}

void run(ByteVector instructions)
{

}

int main()
{
	string filename = "../examples/main.wasp";
	ByteVector instructions = build(filename);
	run(instructions);

	return 0;
}
