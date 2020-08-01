#pragma once
#include <string>
#include <chrono>
#include <memory>
#include "spdlog.h"
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"

using namespace std::chrono;
using std::make_unique;

int main()
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");
	spdlog::info("Welcome to Wasp!");

	auto start = high_resolution_clock::now();

	std::wstring raw_source = read_source("../examples/main.txt");

	auto lexer = make_unique<Lexer>();
	auto tokens = lexer->execute(raw_source);

	auto parser = make_unique<Parser>();
	auto ast = parser->execute(tokens);

	auto semantic_analyser = make_unique<SemanticAnalyzer>();
	semantic_analyser->execute(ast);

	// Calculate time taken

	auto end = high_resolution_clock::now();
	auto execution_time = duration_cast<milliseconds>(end - start).count();
	spdlog::info("Execution Time : {} milliseconds", execution_time);

	return 0;
}