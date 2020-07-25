#pragma once
#include <string>
#include <chrono>
#include "spdlog.h"
#include "utils.h"
#include "Lexer.h"

using namespace std::chrono;

int main()
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");
	spdlog::info("Welcome to Wasp!");

	auto start = high_resolution_clock::now();

	std::wstring raw_source = read_source("../examples/main.txt");

	Lexer lexer;
	auto tokens = lexer.execute(raw_source);

	// Calculate time taken

	auto end = high_resolution_clock::now();
	auto execution_time = duration_cast<milliseconds>(end - start).count();
	spdlog::info("Execution Time : {} milliseconds", execution_time);

	return 0;
}