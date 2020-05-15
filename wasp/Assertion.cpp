#pragma once
#include "Assertion.h"
#include <string>
#include "spdlog.h"

void ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		spdlog::error(message);
		exit(1);
	}
}

void FATAL(std::string message)
{
	spdlog::error(message);
	exit(1);
}