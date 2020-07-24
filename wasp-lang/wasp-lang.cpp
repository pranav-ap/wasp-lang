#pragma once
#include <string>
#include <chrono>
#include "spdlog.h"
#include "utils.h"

using namespace std::chrono;

int main()
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");
	spdlog::info("Welcome to Wasp!");
	auto start = high_resolution_clock::now();

	STRING raw_source = Utils::read_source("../examples/main.txt");

	// Calculate time taken

	auto end = high_resolution_clock::now();
	auto time_taken = std::chrono::duration_cast<milliseconds>(end - start).count();
	spdlog::info("Time Taken : {} milliseconds", time_taken);

	return 0;
}