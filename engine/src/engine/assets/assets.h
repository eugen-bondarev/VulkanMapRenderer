#pragma once

#include <string>

namespace Engine
{
	namespace Assets
	{
		extern std::string root;

		void LocateRoot(int amountOfArguments, char *arguments[]);
	}
}