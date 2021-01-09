#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "utf8.h"

UTILS_API std::wstring read_source(std::string path);
UTILS_API std::wstring concat(std::vector<std::wstring> items, std::wstring connector);

