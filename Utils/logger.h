#pragma once
#include "rang.hpp"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string>

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#define INFO(text) Logger::info(text)
#define TRACE(text) Logger::trace(text)
#define ERROR(text) Logger::error(text)
#define FATAL(text) Logger::fatal(text)

#define FATAL_IF_NULLPTR(x, text) if (!x) { Logger::fatal(text); }
#define FATAL_IF_TRUE(x, text) if (x) { Logger::fatal(text); }
#define FATAL_IF_FALSE(x, text) if (!x) { Logger::fatal(text); }

namespace Logger
{
	void UTILS_API log(rang::bgB bg, std::string type, std::string text);

	void UTILS_API info(std::string text);
	void UTILS_API fatal(std::string text);
	void UTILS_API trace(std::string text);
	void UTILS_API error(std::string text);
};
