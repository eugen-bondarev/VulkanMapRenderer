#pragma once

#include "renderer/atlas/tile_map.h"
#include "renderer/atlas/texture_atlas.h"

#include <engine/engine.h>

#include "components/map/map.h"
#include "components/camera/camera.h"

struct UBOScene
{
	glm::mat4x4 projection;
};

namespace Offscreen
{		
	class ColorPass
	{
	public:
		ColorPass(Map* map, Camera* camera, Engine::Vk::DescriptorPool* descriptor_pool);

		VkDescriptorImageInfo& GetOutputDescriptorImageInfo();

		void WriteToCmd(Engine::Vk::CommandBuffer* cmd, int instances_of_walls, int instances_of_blocks);
		void UpdateBlocks(const std::vector<glm::vec4>& render_data);
		void UpdateWalls(const std::vector<glm::vec4>& render_data);
		void UpdateSpace();

		struct 
		{
			std::shared_ptr<Engine::Vk::Buffer> vertexBuffer;
			std::shared_ptr<Engine::Vk::Buffer> indexBuffer;
			std::shared_ptr<BlocksTileMap> tileMap;
		} block;

	private:
		Map* map;
		Camera* camera;

		std::shared_ptr<Engine::Vk::Pipeline> pipeline;
		std::shared_ptr<Engine::Vk::Texture2D> texture;
		std::shared_ptr<Engine::Vk::Framebuffer> framebuffer;

		std::shared_ptr<Engine::Vk::DescriptorSetLayout> descriptorSetLayout;
		std::shared_ptr<Engine::Vk::DescriptorSet> descriptorSet;

		struct
		{
			struct 
			{
				std::shared_ptr<Engine::Vk::Buffer> buffer;
				UBOScene data;
			} perScene;
		} ubo;
		
		std::shared_ptr<Engine::Vk::Buffer> blocksVertexBuffer;
		std::shared_ptr<Engine::Vk::Buffer> wallsVertexBuffer;
	};
}