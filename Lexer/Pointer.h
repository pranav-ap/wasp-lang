#pragma once

#ifdef LEXER_EXPORTS
#define LEXER_API __declspec(dllexport)
#else
#define LEXER_API __declspec(dllimport)
#endif

class LEXER_API Pointer
{
	int index;
public:
	Pointer() : index(0) {};

	void advance();
	void retreat();
	int get_index() const;
};
