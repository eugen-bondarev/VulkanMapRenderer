#include <engine/engine.h>

#include "game/world/map/map.h"
#include "game/ecs/camera.h"
#include "game/game.h"

#include "renderer/atlas/texture_atlas.h"
#include "renderer/atlas/tile_map.h"

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
	std::vector<glm::vec4> renderData;

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
		struct 
		{
			Vk::Buffer* vertexBuffer;
			Vk::Buffer* indexBuffer;
		} mesh;
		
		Vk::Buffer* dynamicVertexBuffer;

		struct
		{
			struct 
			{
				Vk::Buffer* buffer;
				UBOScene data;
			} perScene;
		} ubo;

		Vk::Pipeline* pipeline;		
	} scene;

	std::shared_ptr<BlocksTileMap> tileMap;

	Vk::DescriptorPool* descriptorPool;
	Vk::DescriptorSetLayout* descriptorSetLayout;
	Vk::DescriptorSet* descriptorSet;
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}