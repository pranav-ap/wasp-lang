#pragma once
#include "pch.h"
#include "Assertion.h"
#include <string>
#include <iostream>

void ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		FATAL(message);
	}
}

void FATAL(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}