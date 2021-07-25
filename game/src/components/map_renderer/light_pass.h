#pragma once

#include "renderer/atlas/tile_map.h"
#include "renderer/atlas/texture_atlas.h"

#include <engine/engine.h>

#include "components/map/map.h"
#include "components/camera/camera.h"

struct UBOScene1
{
	glm::mat4x4 projection;
};

namespace Offscreen
{		
	class LightPass
	{
	public:
		LightPass(Map* map, Camera* camera, Engine::Vk::DescriptorPool* descriptor_pool);

		VkDescriptorImageInfo& GetOutputDescriptorImageInfo();

		void WriteToCmd(Engine::Vk::CommandBuffer* cmd, int instances);
		void UpdateBlocks(const std::vector<glm::vec2>& render_data);
		void UpdateSpace();

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
			std::shared_ptr<Engine::Vk::Buffer> vertexBuffer;
			std::shared_ptr<Engine::Vk::Buffer> indexBuffer;
			std::shared_ptr<Engine::Vk::Texture2D> texture;
		} light;

		struct
		{
			struct 
			{
				std::shared_ptr<Engine::Vk::Buffer> buffer;
				UBOScene1 data;
			} perScene;
		} ubo;
		
		std::shared_ptr<Engine::Vk::Buffer> dynamicVertexBuffer;
	};
}