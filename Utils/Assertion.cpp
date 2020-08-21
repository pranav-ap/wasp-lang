#pragma once
#include "pch.h"
#include "Assertion.h"
#include "ErrorCode.h"
#include <string>
#include <iostream>

void ASSERT(bool condition, ERROR_CODE error_code)
{
	if (!condition)
	{
		FATAL(error_code);
		exit(1);
	}
}

void ASSERT(bool condition, std::string message)
{
	if (!condition)
	{
		FATAL(message);
	}
}

void FATAL(ERROR_CODE error_code)
{
	exit(1);
}

void FATAL(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}