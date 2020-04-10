#pragma once

#include "ObjectSystem.h"
#include <string>
#include <vector>

namespace io
{
	// ECHO

	ObjectVariant_ptr echo_visit(std::vector<ObjectVariant_ptr> arguments);
	ObjectVariant_ptr echo(std::string text);

	// ASK

	ObjectVariant_ptr ask_visit(std::vector<ObjectVariant_ptr> arguments);
	ObjectVariant_ptr ask(std::string& content);
	ObjectVariant_ptr ask(double& number);
}
