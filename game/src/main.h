#include <engine/engine.h>

using namespace Engine;

struct UBOScene
{
	glm::mat4x4 view;
	glm::mat4x4 projection;
};

struct UBOInstance
{
	Aligned<glm::mat4x4> model;

	UBOInstance(uint32_t amount_of_instances) : model { amount_of_instances }
	{

	}
};

class NaturaForge : public App
{
public:

	int amountOfInstances = 4;

	void Init() override;
	void UpdateUBO();
	void RecordCommandBuffer(Vk::CommandPool* command_pool, Vk::CommandBuffer* cmd, Vk::Framebuffer* framebuffer);
	void Render(Vk::CommandBuffer* cmd);
	void Present();
	void Update() override;
	void Shutdown() override;

	Vk::FrameManager* frameManager;
	std::vector<Vk::CommandPool*> commandPools;
	std::vector<Vk::CommandBuffer*> commandBuffers;
	std::vector<VkFence> imagesInFlight;
	std::vector<Vk::Framebuffer*> framebuffers;

	struct
	{
		struct 
		{
			Vk::Buffer* vertexBuffer;
			Vk::Buffer* indexBuffer;
		} mesh;

		struct
		{
			struct 
			{
				Vk::Buffer* buffer;
				UBOScene data;
			} perScene;

			struct
			{
				Vk::Buffer* buffer;
				UBOInstance* data;
			} perInstance;
		} ubo;

		Vk::Pipeline* pipeline;		
	} scene;

	Vk::DescriptorPool* descriptorPool;

	Vk::DescriptorSetLayout* descriptorSetLayout;
	Vk::DescriptorSet* descriptorSet;


};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}