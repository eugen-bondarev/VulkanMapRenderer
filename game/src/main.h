#include <engine/engine.h>

#include "components/camera/camera.h"
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

	void UpdateProjectionViewMatrix();

	void Render(Vk::CommandBuffer* cmd);
	
	void Present();

	Vk::FrameManager* frameManager;
	std::vector<VkFence> imagesInFlight;
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}