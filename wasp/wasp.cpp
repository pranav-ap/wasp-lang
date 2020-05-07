#pragma once
#include "WaspCLI.h"
#include "spdlog.h"

int main(int argc, char** argv)
{
	spdlog::set_pattern("%^[%=8l]%$ %@ %v");

	WaspCLI cli;
	CLI11_PARSE(cli.get_app(), argc, argv);
}