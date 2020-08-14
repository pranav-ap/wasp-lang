﻿#pragma once
#include <string>
#include <chrono>
#include <memory>
#include "spdlog.h"
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"
#include "Compiler.h"

using namespace std::chrono;
using std::make_unique;

int main()
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");
	spdlog::info("Welcome to Wasp!");

	auto start = high_resolution_clock::now();

	std::wstring raw_source = read_source("../examples/main.txt");

	Lexer_ptr lexer = make_unique<Lexer>();
	std::vector<Token_ptr> tokens = lexer->execute(raw_source);

	Parser_ptr parser = make_unique<Parser>();
	Module_ptr ast = parser->execute(tokens);

	SemanticAnalyzer_ptr semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->execute(ast);

	Compiler_ptr compiler = make_unique<Compiler>();
	Bytecode_ptr bytecode = compiler->execute(ast);

	// Calculate time taken

	auto end = high_resolution_clock::now();
	auto execution_time = duration_cast<milliseconds>(end - start).count();
	spdlog::info("Execution Time : {} milliseconds", execution_time);

	return 0;
}