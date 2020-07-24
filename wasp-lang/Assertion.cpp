#pragma once
#include "Assertion.h"
#include <string>
#include "spdlog.h"

void Utils::ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		FATAL(message);
	}
}

void Utils::FATAL(std::string message)
{
	spdlog::error(message);
	exit(1);
}