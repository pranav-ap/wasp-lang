#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#include <string>

UTILS_API std::string read_source(std::string path);
