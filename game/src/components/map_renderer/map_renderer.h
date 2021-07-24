#pragma once

#include "ecs/ecs.h"

#include "../map/map.h"

#include "renderer/atlas/tile_map.h"
#include "color_pass.h"
#include "composition.h"

class MapRenderer : public Component
{
public:
	MapRenderer();
	~MapRenderer();
	void Init(Map* map, Camera* camera, Engine::Vk::DescriptorPool* descriptor_pool);

	void Update();
	void UpdateSpace();

	void FillCommandBuffers();
	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render);

	void Render(Engine::Vk::Frame* frame);

	std::vector<Engine::Vk::CommandBuffer*>& GetCommandBuffers();
	Engine::Vk::CommandBuffer* GetCurrentCmd();

private:
	Map* map;
	Camera* camera;

	std::vector<Engine::Vk::CommandBuffer*> commandBuffers;
	Offscreen::ColorPass* colorPass;
	Composition* composition;

	MapRenderer(const MapRenderer&) = delete;
	MapRenderer& operator=(const MapRenderer&) = delete;
};