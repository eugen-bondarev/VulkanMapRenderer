#include "assets.h"

namespace Engine
{
	namespace Assets
	{
		std::string root;

		void LocateRoot(int amountOfArguments, char *arguments[])
		{
			std::string path = std::string(arguments[0]);
			size_t slash0 = path.find_last_of('\\');
			size_t slash1 = path.find_last_of('/');
			size_t slashIndex = (slash0 != std::string::npos ? slash0 : slash1);
			root = path.substr(0, slashIndex) + "/";
		}
	}
}