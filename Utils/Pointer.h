#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif

class UTILS_API Pointer
{
	int index;
public:
	Pointer() : index(0) {};
	void advance();
	void retreat();

	int get_index() const;
};
