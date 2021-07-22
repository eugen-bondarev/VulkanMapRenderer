#pragma once

#include <engine/engine.h>

namespace MapRenderer
{
	class Composition
	{
	public:
		Composition(Engine::Vk::DescriptorPool* descriptor_pool);

		void WriteToCmd(Engine::Vk::CommandBuffer* cmd, Engine::Vk::Framebuffer* framebuffer);

	private:		
		std::shared_ptr<Engine::Vk::Pipeline> pipeline;
		std::shared_ptr<Engine::Vk::DescriptorSetLayout> descriptorSetLayout;
		std::shared_ptr<Engine::Vk::DescriptorSet> descriptorSet;

		struct 
		{
			std::shared_ptr<Engine::Vk::Buffer> vertexBuffer;
			std::shared_ptr<Engine::Vk::Buffer> indexBuffer;
		} canvas;
	};
}