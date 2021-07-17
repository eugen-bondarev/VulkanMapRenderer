#include "text_asset.h"

#include "assets.h"

#include <fstream>

namespace Engine
{
	namespace Assets
	{
		Text::Text(const std::string &path)
		{
			std::string fullPath = root + path;
			std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				ERR_OUT("Failed to open a file: {0}", fullPath);
			}

			size_t fileSize = (size_t)file.tellg();
			content.resize(sizeof(char) * fileSize);

			file.seekg(0);
			file.read(content.data(), fileSize);

			file.close();
		}

		const std::string &Text::GetContent() const
		{
			return content;
		}
	}
}