#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#include <string>

#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")

UTILS_API void ASSERT(bool condition, std::string message);
UTILS_API void FATAL(std::string message);
