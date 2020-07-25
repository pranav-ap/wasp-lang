#pragma once
#include <string>
#include <chrono>
#include <memory>
#include "spdlog.h"
#include "utils.h"
#include "Lexer.h"
#include "Parser.h"

using namespace std::chrono;

int main()
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");
	spdlog::info("Welcome to Wasp!");

	auto start = high_resolution_clock::now();

	std::wstring raw_source = read_source("../examples/main.txt");

	auto lexer = std::make_unique<Lexer>();
	auto tokens = lexer->execute(raw_source);

	auto parser = std::make_unique<Parser>();
	auto ast = parser->execute(tokens);

	// Calculate time taken

	auto end = high_resolution_clock::now();
	auto execution_time = duration_cast<milliseconds>(end - start).count();
	spdlog::info("Execution Time : {} milliseconds", execution_time);

	return 0;
}