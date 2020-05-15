#pragma once
#include "CLI11.hpp"
#include <string>

class WaspCLI
{
	CLI::App app;
	std::string filepath;

	void run();

public:

	WaspCLI();
	CLI::App& get_app();
};
