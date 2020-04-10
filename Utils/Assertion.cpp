#pragma once
#include "pch.h"
#include "Assertion.h"

void Assertion::ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		spdlog::error(message);
		exit(1);
	}
}

void Assertion::FATAL(std::string message)
{
	spdlog::error(message);
	exit(1);
}