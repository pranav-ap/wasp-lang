#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#include "ErrorCode.h"
#include <string>

UTILS_API void ASSERT(bool condition, ERROR_CODE error_code);
UTILS_API void ASSERT(bool condition, std::string message);

UTILS_API void FATAL(ERROR_CODE error_code);
UTILS_API void FATAL(std::string message);
