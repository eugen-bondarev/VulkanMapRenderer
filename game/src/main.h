#include <engine/engine.h>

#include "game/world/map/map.h"
#include "game/ecs/camera.h"
#include "game/game.h"

#include "renderer/atlas/texture_atlas.h"
#include "renderer/atlas/tile_map.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

using namespace Engine;

class NaturaForge : public App
{
public:
	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	std::shared_ptr<Game> game;

	void UpdateMap();
	void UpdateProjectionViewMatrix();

	void RenderUI();
	void Render(Vk::CommandBuffer* cmd);
	
	void Present();

	Vk::FrameManager* frameManager;
	std::vector<Vk::CommandBuffer*> commandBuffers;
	std::vector<VkFence> imagesInFlight;

	struct
	{
		Vk::DescriptorPool* descriptorPool;
	} common;

	void InitCommonResources();

	void FillCommandBuffers();

	void InitImGui();
	void ShutdownImGui();

	struct
	{
		Vk::RenderPass* renderPass;
		std::vector<Vk::CommandPool*> commandPools;
		std::vector<Vk::CommandBuffer*> commandBuffers;
	} imgui;
	void FillImGuiCommandBuffers();	
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}