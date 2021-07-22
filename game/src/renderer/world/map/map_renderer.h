#pragma once

#include "game/world/map/map.h"
#include "game/game.h"
#include "renderer/atlas/tile_map.h"

#include <engine/engine.h>

struct UBOScene
{
	glm::mat4x4 projection;
};

namespace MapRenderer
{
	class Offscreen
	{
	public:
		Offscreen(const std::shared_ptr<Game>& game, Engine::Vk::DescriptorPool* descriptor_pool);

		VkDescriptorImageInfo& GetOutputDescriptorImageInfo();

		void WriteToCmd(Engine::Vk::CommandBuffer* cmd);
		void UpdateBlocks();
		void UpdateSpace();

	private:
		const std::shared_ptr<Game>& game;

		std::shared_ptr<Engine::Vk::Pipeline> pipeline;
		std::shared_ptr<Engine::Vk::Texture2D> texture;
		std::shared_ptr<Engine::Vk::Framebuffer> framebuffer;

		std::shared_ptr<Engine::Vk::DescriptorSetLayout> descriptorSetLayout;
		std::shared_ptr<Engine::Vk::DescriptorSet> descriptorSet;

		struct 
		{
			std::shared_ptr<Engine::Vk::Buffer> vertexBuffer;
			std::shared_ptr<Engine::Vk::Buffer> indexBuffer;
			std::shared_ptr<BlocksTileMap> tileMap;
		} block;

		struct
		{
			struct 
			{
				std::shared_ptr<Engine::Vk::Buffer> buffer;
				UBOScene data;
			} perScene;
		} ubo;
		
		std::shared_ptr<Engine::Vk::Buffer> dynamicVertexBuffer;
	};

	extern Offscreen* offscreen;

	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data);
}