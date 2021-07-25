#pragma once

#include "ecs/ecs.h"

#include <engine/engine.h>

class UIRenderer : public Component, public Collections::IRenderable
{
public:
	UIRenderer();
	~UIRenderer();

	virtual void Render(Engine::Vk::Frame* frame) override;
	
private:
	Engine::Vk::RenderPass* renderPass;
	std::vector<Engine::Vk::CommandPool*> commandPools;
	std::vector<Engine::Vk::CommandBuffer*> commandBuffers;

	UIRenderer(const UIRenderer&) = delete;	
	UIRenderer& operator=(const UIRenderer&) = delete;	
};