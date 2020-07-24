#pragma once
#include <string>
#include <vector>
#include "utf8.h"

using CHARACTER = wchar_t;
using STRING = std::wstring;
using DECIMAL_CODE_POINT = unsigned short;
using DECIMAL_CODE_POINT_VECTOR = std::vector<DECIMAL_CODE_POINT>;

namespace Utils
{
	STRING read_source(std::string path);
}
