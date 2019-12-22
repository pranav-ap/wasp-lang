#pragma once

class Position {
public:
	int line_num;
	int column_num;
	
	Position() : line_num(1), column_num(1) {};

	void increment_line_number();
	void increment_column_number();
	void reset_column_number();
};