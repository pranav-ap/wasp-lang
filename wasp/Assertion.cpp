#pragma once
#include "Assertion.h"
#include <string>
#include "spdlog.h"

void ASSERT(bool condition, std::wstring message)
{
	if (!condition)
	{
		FATAL(message);
	}
}

void FATAL(std::wstring message)
{
	spdlog::error(message);
	exit(1);
}