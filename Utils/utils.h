#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include "utf8.h"

using DECIMAL_CODE_POINT = unsigned short;
using DECIMAL_CODE_POINT_VECTOR = std::vector<DECIMAL_CODE_POINT>;

UTILS_API std::wstring read_source(std::string path);
