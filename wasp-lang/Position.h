#pragma once
#include <string>

class Position {
	int line_number;
	int column_number;
public:
	Position() : line_number(1), column_number(1) {};
	void increment_line_number();
	void increment_column_number();
	void reset_column_number();
	std::string to_string();
};
