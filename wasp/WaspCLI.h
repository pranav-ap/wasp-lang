#pragma once
#include "CLI11.hpp"
#include <string>

class WaspCLI
{
	CLI::App app;

	std::string filepath;
	bool provide_trace;

	void run();

public:

	WaspCLI();
	CLI::App& get_app();
};
