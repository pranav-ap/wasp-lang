#pragma once
#include "pch.h"
#include "Assertion.h"
#include <string>
#include "spdlog.h"

void ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		FATAL(message);
	}
}

void FATAL(std::string message)
{
	spdlog::error(message);
	exit(1);
}