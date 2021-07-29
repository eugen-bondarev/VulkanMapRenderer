#pragma once

#include "ecs/ecs.h"

#include <engine/engine.h>

class MenuRenderer : public Component, public Collections::IRenderable
{
public:
	MenuRenderer();
	~MenuRenderer();

	virtual void Render(Engine::Vk::Frame* frame) override;
	
private:
	Engine::Vk::RenderPass* renderPass;
	std::vector<Engine::Vk::CommandPool*> commandPools;
	std::vector<Engine::Vk::CommandBuffer*> commandBuffers;

	MenuRenderer(const MenuRenderer&) = delete;	
	MenuRenderer& operator=(const MenuRenderer&) = delete;	
};