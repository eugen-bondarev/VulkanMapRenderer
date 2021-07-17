#pragma once

#include <string>

#include <glm/vec2.hpp>

namespace Engine
{
	namespace Assets
	{
		class Image
		{
		public:
			Image(const std::string &path);
			~Image();

			const unsigned char* GetData() const;
			glm::vec2 GetSize() const;
			int GetAmountOfChannels() const;

		private:
			unsigned char* data { nullptr };
			glm::vec2 size;
			int amountOfChannels;
		};
	}
}