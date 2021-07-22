#pragma once

#include "game/world/map/map.h"
#include "game/game.h"
#include "renderer/atlas/tile_map.h"

#include "color_pass.h"
#include "composition.h"

class MapRenderer
{
public:
	MapRenderer(Engine::Vk::DescriptorPool* descriptor_pool, const std::shared_ptr<Game>& game);
	~MapRenderer();

	void Update();
	void UpdateSpace();

	void FillCommandBuffers();
	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data);

	void Render(Engine::Vk::Frame* frame);

	std::vector<Engine::Vk::CommandBuffer*>& GetCommandBuffers();
	Engine::Vk::CommandBuffer* GetCurrentCmd();

private:
	const std::shared_ptr<Game>& game;

	std::vector<Engine::Vk::CommandBuffer*> commandBuffers;
	Offscreen::ColorPass* colorPass;
	Composition* composition;

	MapRenderer(const MapRenderer&) = delete;
	MapRenderer& operator=(const MapRenderer&) = delete;
};

extern MapRenderer* mapRenderer;