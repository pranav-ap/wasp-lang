#pragma once
#include <string>

#define NULL_CHECK(x) Utils::ASSERT(x != nullptr, "Oh shit! A nullptr")

namespace Utils
{
	void ASSERT(bool condition, std::string message);
	void FATAL(std::string message);
}
