#pragma once

class Pointer
{
	int index;
public:
	Pointer() : index(0) {};

	void advance();
	void retreat();
	int get_index() const;
};
