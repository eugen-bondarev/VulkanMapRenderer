#include "main.h"

void NaturaForge::Init()
{
	scene.ubo.perScene.data.view = glm::mat4x4(1);
	scene.ubo.perScene.data.projection = glm::perspective(glm::radians(70.0f), window->GetSize().x / window->GetSize().y, 0.1f, 1000.0f);

	scene.ubo.perInstance.data = new UBOInstance(amountOfInstances);
	size_t buffer_size = amountOfInstances * Aligned<glm::mat4x4>::dynamicAlignment;

	frameManager = new Vk::FrameManager();

	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* new_command_pool = new Vk::CommandPool();
		commandPools.push_back(new_command_pool);
		commandBuffers.push_back(new Vk::CommandBuffer(new_command_pool));
	}

	std::vector<VkDescriptorSetLayoutBinding> bindings = 
	{
		Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
		Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
	};

	descriptorSetLayout = new Vk::DescriptorSetLayout(bindings);

	glm::vec2 viewport_size = { Vk::Global::swapChain->GetExtent().width, Vk::Global::swapChain->GetExtent().height };

	Assets::Text vs_code("assets/shaders/default.vert.spv");
	Assets::Text fs_code("assets/shaders/default.frag.spv");

	scene.pipeline = new Vk::Pipeline(
		vs_code.GetContent(), fs_code.GetContent(), 
		viewport_size, Vk::Global::swapChain->GetImageFormat(), 
		Vk::Vertex::GetBindingDescriptions(), Vk::Vertex::GetAttributeDescriptions(),
		{ descriptorSetLayout->GetVkDescriptorSetLayout() }
	);

	{			
		const std::vector<Vk::Vertex> vertices = 
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		Vk::Buffer staging_buffer(vertices);
		scene.mesh.vertexBuffer = new Vk::Buffer(&staging_buffer);
	}
	{
		const std::vector<uint16_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		Vk::Buffer staging_buffer(indices);
		scene.mesh.indexBuffer = new Vk::Buffer(&staging_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}

	descriptorPool = new Vk::DescriptorPool({
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 }
	});

	scene.ubo.perScene.buffer = new Vk::Buffer(
		sizeof(UBOScene),
		1,
		&scene.ubo.perScene.data,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	);

	scene.ubo.perInstance.buffer = new Vk::Buffer(
		buffer_size,
		1,
		scene.ubo.perInstance.data->model.data,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	);
	scene.ubo.perInstance.buffer->SetDescriptor(Aligned<glm::mat4x4>::dynamicAlignment);

	descriptorSet = new Vk::DescriptorSet(descriptorPool, { descriptorSetLayout->GetVkDescriptorSetLayout() });
	std::vector<VkWriteDescriptorSet> write_descriptor_sets = 
	{
		Vk::CreateWriteDescriptorSet(descriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &scene.ubo.perScene.buffer->GetDescriptor()),
		Vk::CreateWriteDescriptorSet(descriptorSet, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &scene.ubo.perInstance.buffer->GetDescriptor())
	};

	descriptorSet->Update(write_descriptor_sets);

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());
	for (const VkImageView& image_view : Vk::Global::swapChain->GetImageViews())
		framebuffers.push_back(new Vk::Framebuffer(image_view, scene.pipeline->GetRenderPass()->GetVkRenderPass(), viewport_size));
}

void NaturaForge::UpdateUBO()
{
	static float alpha = 0.0f;
	alpha += 0.0001f;

	scene.ubo.perInstance.data->model[0] = glm::translate(glm::mat4x4(1), glm::vec3(0 + alpha, 0, -10));
	scene.ubo.perInstance.data->model[1] = glm::translate(glm::mat4x4(1), glm::vec3(0 - alpha, 0, -10));
	scene.ubo.perInstance.data->model[2] = glm::translate(glm::mat4x4(1), glm::vec3(0, 0 - alpha, -10));
	scene.ubo.perInstance.data->model[3] = glm::translate(glm::mat4x4(1), glm::vec3(0, 0 + alpha, -10));

	scene.ubo.perInstance.buffer->Update(scene.ubo.perInstance.data->model.data);
}

void NaturaForge::RecordCommandBuffer(Vk::CommandPool* command_pool, Vk::CommandBuffer* cmd, Vk::Framebuffer* framebuffer)
{
	command_pool->Reset();

	cmd->Begin();
		cmd->BeginRenderPass(scene.pipeline->GetRenderPass(), framebuffer);
			cmd->BindPipeline(scene.pipeline);
				cmd->BindVertexBuffers({ scene.mesh.vertexBuffer }, { 0 });
				cmd->BindIndexBuffer(scene.mesh.indexBuffer);					
					for (int i = 0; i < amountOfInstances; i++)
					{
						uint32_t dynamic_offset = i * Aligned<glm::mat4x4>::dynamicAlignment;
						cmd->BindDescriptorSets(scene.pipeline, 1, &descriptorSet->GetVkDescriptorSet(), 1, &dynamic_offset);							
						cmd->DrawIndexed(scene.mesh.indexBuffer->GetAmountOfElements(), 1, 0, 0, 0);
					}
		cmd->EndRenderPass();
	cmd->End();
}

void NaturaForge::Render(Vk::CommandBuffer* cmd)
{		
	Vk::Frame* current_frame = frameManager->GetCurrentFrame();

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence());

	cmd->SubmitToQueue(
		Vk::Global::Queues::graphicsQueue, 
		&current_frame->GetImageAvailableSemaphore(), 
		&current_frame->GetRenderFinishedSemaphore(), 
		current_frame->GetInFlightFence()
	);
}

void NaturaForge::Present()
{
	VkResult result = Vk::Global::swapChain->Present(&frameManager->GetCurrentFrame()->GetRenderFinishedSemaphore(), 1);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
	{
		
	} 
	else if (result != VK_SUCCESS) 
	{
		THROW("Can't present.");
	}

	frameManager->NextFrame();
}

void NaturaForge::Update()
{		
	Vk::Frame* current_frame = frameManager->GetCurrentFrame();

	uint32_t image_index = Vk::Global::swapChain->AcquireImage(current_frame->GetImageAvailableSemaphore());

	if (imagesInFlight[image_index] != VK_NULL_HANDLE) 
		vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &imagesInFlight[image_index], VK_TRUE, UINT64_MAX);

	vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence(), VK_TRUE, UINT64_MAX);
	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence());

	imagesInFlight[image_index] = current_frame->GetInFlightFence();

	Vk::CommandPool* current_command_pool = commandPools[image_index];
	Vk::CommandBuffer* current_command_buffer = commandBuffers[image_index];	
	Vk::Framebuffer* current_framebuffer = framebuffers[image_index];

	UpdateUBO();
	RecordCommandBuffer(current_command_pool, current_command_buffer, current_framebuffer);
	Render(current_command_buffer);
	Present();
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();


	delete descriptorSet;
	delete descriptorPool;
	delete descriptorSetLayout;

	for (int i = 0; i < framebuffers.size(); i++)
		delete framebuffers[i];

	delete scene.ubo.perInstance.data;
	delete scene.ubo.perScene.buffer;
	delete scene.ubo.perInstance.buffer;
	delete scene.mesh.vertexBuffer;
	delete scene.mesh.indexBuffer;
	delete scene.pipeline;

	VK_ASSERT(commandBuffers.size() == commandPools.size());
	for (int i = 0; i < commandBuffers.size(); i++)
	{
		delete commandBuffers[i];
		delete commandPools[i];
	}

	delete frameManager;
}

ENTRY()