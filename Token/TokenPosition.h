#pragma once

#ifdef TOKEN_EXPORTS
#define TOKEN_API __declspec(dllexport)
#else
#define TOKEN_API __declspec(dllimport)
#endif

class TOKEN_API TokenPosition
{
	int line_num;
	int column_num;

public:
	TokenPosition() : line_num(1), column_num(1) {};

	void increment_line_number();
	void increment_column_number();
	void reset_column_number();

	int get_line_num() const;
	int get_column_num() const;
};