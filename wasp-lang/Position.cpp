#include "Position.h"

void Position::increment_line_number()
{
	this->line_number++;
}

void Position::increment_column_number()
{
	this->column_number++;
}

void Position::reset_column_number()
{
	this->column_number = 1;
}

std::string Position::to_string()
{
	std::string p;
	p.append("Ln : ");
	p.append(std::to_string(this->line_number));
	p.append(" Col : ");
	p.append(std::to_string(this->column_number));

	return p;
}