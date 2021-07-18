#include "main.h"

void NaturaForge::Init()
{
	scene.ubo.perScene.data.view = glm::mat4x4(1);
	// scene.ubo.perScene.data.projection = glm::perspective(glm::radians(70.0f), window->GetSize().x / window->GetSize().y, 0.1f, 1000.0f);
	glm::vec2 half_size = window->GetSize() / 2.0f;
	scene.ubo.perScene.data.projection = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y);

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
		Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC),
		Vk::CreateBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayout = new Vk::DescriptorSetLayout(bindings);

	{
		const Assets::Image map_texture("assets/textures/map.png");
		uint32_t buffer_size = map_texture.GetSize().x * map_texture.GetSize().y * map_texture.GetAmountOfChannels();
		Vk::Buffer staging_buffer(map_texture.GetData(), buffer_size);

		image = new Vk::Image(&staging_buffer, map_texture.GetSize(), map_texture.GetAmountOfChannels());
		imageView = new Vk::ImageView(image);
		sampler = new Vk::Sampler();
	}

	Assets::Text vs_code("assets/shaders/default.vert.spv");
	Assets::Text fs_code("assets/shaders/default.frag.spv");

	Vk::BindingDescriptions vertex_buffer_binding_descriptors;
	Vk::AttributeDescriptions vertex_buffer_attribute_descriptors;
	for (int i = 0; i < Vk::Vertex::GetBindingDescriptions().size(); i++)
		vertex_buffer_binding_descriptors.push_back(Vk::Vertex::GetBindingDescriptions()[i]);
	for (int i = 0; i < Vk::PerInstanceVertex::GetBindingDescriptions().size(); i++)
		vertex_buffer_binding_descriptors.push_back(Vk::PerInstanceVertex::GetBindingDescriptions()[i]);
	for (int i = 0; i < Vk::Vertex::GetAttributeDescriptions().size(); i++)
		vertex_buffer_attribute_descriptors.push_back(Vk::Vertex::GetAttributeDescriptions()[i]);
	for (int i = 0; i < Vk::PerInstanceVertex::GetAttributeDescriptions().size(); i++)
		vertex_buffer_attribute_descriptors.push_back(Vk::PerInstanceVertex::GetAttributeDescriptions()[i]);

	scene.pipeline = new Vk::Pipeline(
		vs_code.GetContent(), fs_code.GetContent(), 
		ExtentToVec2(Vk::Global::swapChain->GetExtent()), Vk::Global::swapChain->GetImageFormat(), 
		vertex_buffer_binding_descriptors, vertex_buffer_attribute_descriptors,
		{ descriptorSetLayout->GetVkDescriptorSetLayout() }
	);
	scene.pipeline->SetAsOutput();

	{
		scene.dynamicVertexBuffer = new Vk::Buffer(sizeof(Vk::PerInstanceVertex), 25, nullptr, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	}
	{			
		const std::vector<Vk::Vertex> vertices = 
		{
			{{-8.0f, -8.0f}, { 0.0f, 0.0f }},
			{{ 8.0f, -8.0f}, { 1.0f, 0.0f }},
			{{ 8.0f,  8.0f}, { 1.0f, 1.0f }},
			{{-8.0f,  8.0f}, { 0.0f, 1.0f }}
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
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 }
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

	VkWriteDescriptorSet image_write_descriptor_set = {};

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = imageView->GetVkImageView();
    image_info.sampler = sampler->GetVkSampler();

	image_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	image_write_descriptor_set.dstSet = descriptorSet->GetVkDescriptorSet();
	image_write_descriptor_set.dstBinding = 2;
	image_write_descriptor_set.dstArrayElement = 0;
	image_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	image_write_descriptor_set.descriptorCount = 1;
	image_write_descriptor_set.pImageInfo = &image_info;

	std::vector<VkWriteDescriptorSet> write_descriptor_sets = 
	{
		Vk::CreateWriteDescriptorSet(descriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &scene.ubo.perScene.buffer->GetDescriptor()),
		Vk::CreateWriteDescriptorSet(descriptorSet, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, &scene.ubo.perInstance.buffer->GetDescriptor()),
		image_write_descriptor_set
	};

	descriptorSet->Update(write_descriptor_sets);

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());
}

void NaturaForge::UpdateUBO()
{
	static float alpha = 0.0f;
	alpha += 0.001f;

	std::array<glm::vec2, 2> positions;
	positions[0] = glm::vec2(alpha, 0);
	positions[1] = glm::vec2(-alpha, 0);
	scene.dynamicVertexBuffer->Update(positions.data(), static_cast<uint32_t>(sizeof(glm::vec2) * positions.size()));

	scene.ubo.perInstance.data->model[0] = glm::translate(glm::mat4x4(1), glm::vec3(0, 0, 0));
	// scene.ubo.perInstance.data->model[1] = glm::translate(glm::mat4x4(1), glm::vec3(0 - alpha, 0, -10));
	// scene.ubo.perInstance.data->model[2] = glm::translate(glm::mat4x4(1), glm::vec3(0, 0 - alpha, -10));
	// scene.ubo.perInstance.data->model[3] = glm::translate(glm::mat4x4(1), glm::vec3(0, 0 + alpha, -10));

	scene.ubo.perInstance.buffer->Update(scene.ubo.perInstance.data->model.data);
}

void NaturaForge::RecordCommandBuffer(Vk::CommandPool* command_pool, Vk::CommandBuffer* cmd)
{
	command_pool->Reset();

	cmd->Begin();
		cmd->BeginRenderPass(scene.pipeline->GetRenderPass(), Vk::Global::swapChain->GetCurrentScreenFramebuffer());
			cmd->BindPipeline(scene.pipeline);
				cmd->BindVertexBuffers({ scene.mesh.vertexBuffer, scene.dynamicVertexBuffer }, { 0, 0 });
				cmd->BindIndexBuffer(scene.mesh.indexBuffer);					
					for (int i = 0; i < amountOfInstances; i++)
					{
						uint32_t dynamic_offset = i * Aligned<glm::mat4x4>::dynamicAlignment;
						cmd->BindDescriptorSets(scene.pipeline, 1, &descriptorSet->GetVkDescriptorSet(), 1, &dynamic_offset);							
						cmd->DrawIndexed(scene.mesh.indexBuffer->GetAmountOfElements(), 2, 0, 0, 0);
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

	UpdateUBO();
	RecordCommandBuffer(current_command_pool, current_command_buffer);
	Render(current_command_buffer);
	Present();

	static float timer = 0.0f;
	timer += Time::deltaTime;
	
	if (timer >= 1.0f)
	{
		LOG_OUT("FPS: {0}", Time::GetFPS());
		timer = 0.0f;
	}
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();

	delete sampler;
	delete imageView;
	delete image;

	delete descriptorSet;
	delete descriptorPool;
	delete descriptorSetLayout;

	delete scene.ubo.perInstance.data;
	delete scene.ubo.perScene.buffer;
	delete scene.ubo.perInstance.buffer;
	delete scene.mesh.vertexBuffer;
	delete scene.mesh.indexBuffer;
	delete scene.pipeline;

	delete scene.dynamicVertexBuffer;

	VK_ASSERT(commandBuffers.size() == commandPools.size());
	for (int i = 0; i < commandBuffers.size(); i++)
	{
		delete commandBuffers[i];
		delete commandPools[i];
	}

	delete frameManager;
}

ENTRY()