#include "image_asset.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include "assets.h"

namespace Engine
{
	namespace Assets
	{
		Image::Image(const std::string &path)
		{
			int width, height;
			std::string fullPath = root + path;
			data = stbi_load(fullPath.c_str(), &width, &height, &amountOfChannels, 0);
			if (!data)
			{
				ERR_OUT("Failed to open a file: {0}", fullPath);
			}
			size = {static_cast<float>(width), static_cast<float>(height)};
		}

		Image::~Image()
		{
			stbi_image_free(data);
		}

		const unsigned char* Image::GetData() const
		{
			return data;
		}

		glm::vec2 Image::GetSize() const
		{
			return size;
		}

		int Image::GetAmountOfChannels() const
		{
			return amountOfChannels;
		}		
	}
}