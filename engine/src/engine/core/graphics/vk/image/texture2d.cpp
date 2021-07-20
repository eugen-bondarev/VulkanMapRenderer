#include "texture2d.h"

#include "../memory/buffer.h"

namespace Engine
{
	namespace Vk
	{
		Texture2D::Texture2D(glm::vec2 size, int amount_of_channels, const void* data, VkImageUsageFlags usage_flags) : size { size }
		{
			uint32_t buffer_size = size.x * size.y * amount_of_channels;

			Vk::Buffer staging_buffer(data, buffer_size);
			image = new Vk::Image(&staging_buffer, size, amount_of_channels, usage_flags);
			imageView = new Vk::ImageView(image);
		}

		Texture2D::~Texture2D()
		{
			delete imageView;
			delete image;
		}

		Image* Texture2D::GetImage()
		{
			return image;
		}

		ImageView* Texture2D::GetImageView()
		{
			return imageView;
		}
	}
}