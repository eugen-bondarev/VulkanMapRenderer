#pragma once

#include <engine/engine.h>

namespace Programs
{
	namespace MapEditor
	{
		struct InitData
		{
			Engine::Vk::Texture2D* texture;
			glm::vec2 textureSize;
			float tileSize;
		};		

		void Init(InitData init_data);
		void Render();
	}
}