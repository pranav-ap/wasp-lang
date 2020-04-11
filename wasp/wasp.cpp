#pragma once
#include "WaspCLI.h"

int main(int argc, char** argv)
{
	WaspCLI cli;

	CLI11_PARSE(cli.get_app(), argc, argv);
}