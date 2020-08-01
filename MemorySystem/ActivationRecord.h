#pragma once

#ifdef MEMORYSYSTEM_EXPORTS
#define MEMORYSYSTEM_API __declspec(dllexport)
#else
#define MEMORYSYSTEM_API __declspec(dllimport)
#endif

#include <memory>
#include <string>

class MEMORYSYSTEM_API ActivationRecord
{
public:
	std::wstring name;
};

using ActivationRecord_ptr = MEMORYSYSTEM_API std::shared_ptr<ActivationRecord>;
