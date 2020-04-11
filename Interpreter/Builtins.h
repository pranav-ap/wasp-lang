#pragma once

#include "ObjectSystem.h"
#include <string>
#include <vector>

namespace io
{
	ObjectVariant_ptr echo_visit(std::vector<ObjectVariant_ptr> arguments);
	ObjectVariant_ptr ask_visit(std::vector<ObjectVariant_ptr> arguments);
}
