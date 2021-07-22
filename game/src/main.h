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

struct UBOScene
{
	glm::mat4x4 projection;
};

class NaturaForge : public App
{
public:

	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	std::unique_ptr<Game> game;

	void UpdateMap();
	void UpdateProjectionViewMatrix();
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

	struct
	{
		Vk::Pipeline* pipeline;
		Vk::Texture2D* texture;
		Vk::Framebuffer* framebuffer;

		Vk::DescriptorSetLayout* descriptorSetLayout;
		Vk::DescriptorSet* descriptorSet;

		struct 
		{
			Vk::Buffer* vertexBuffer;
			Vk::Buffer* indexBuffer;
		} block;

		struct
		{
			struct 
			{
				Vk::Buffer* buffer;
				UBOScene data;
			} perScene;
		} ubo;
		
		Vk::Buffer* dynamicVertexBuffer;

	} offscreen;

	void InitOffscreenPipelineResources();

	struct
	{
		Vk::Pipeline* pipeline;

		Vk::DescriptorSetLayout* descriptorSetLayout;
		Vk::DescriptorSet* descriptorSet;

		struct 
		{
			Vk::Buffer* vertexBuffer;
			Vk::Buffer* indexBuffer;
		} canvas;

	} composition;

	void InitCompositionPipelineResources();

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

	std::shared_ptr<BlocksTileMap> tileMap;
	
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}