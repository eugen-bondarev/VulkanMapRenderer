#include "main.h"

#include "renderer/world/map/map_renderer.h"

void NaturaForge::Init()
{
	frameManager = new Vk::FrameManager();

	glm::vec2 half_size = window->GetSize() / 2.0f;
	scene.ubo.perScene.data.projection = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y);

	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* new_command_pool = new Vk::CommandPool();
		commandPools.push_back(new_command_pool);
		commandBuffers.push_back(new Vk::CommandBuffer(new_command_pool));
	}

	std::vector<VkDescriptorSetLayoutBinding> bindings = 
	{
		Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
		Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayout = new Vk::DescriptorSetLayout(bindings);

	{
		const Assets::Image map_texture("assets/textures/map.png");
		uint32_t buffer_size = map_texture.GetSize().x * map_texture.GetSize().y * map_texture.GetAmountOfChannels();
		Vk::Buffer staging_buffer(map_texture.GetData(), buffer_size);

		image = new Vk::Image(&staging_buffer, map_texture.GetSize(), map_texture.GetAmountOfChannels());
		imageView = new Vk::ImageView(image);
	}

	Assets::Text vs_code("assets/shaders/default.vert.spv");
	Assets::Text fs_code("assets/shaders/default.frag.spv");

	Vk::BindingDescriptions vertex_buffer_binding_descriptors = Util::Vector::Merge(Vk::Vertex::GetBindingDescriptions(), Vk::PerInstanceVertex::GetBindingDescriptions());
	Vk::AttributeDescriptions vertex_buffer_attribute_descriptors = Util::Vector::Merge(Vk::Vertex::GetAttributeDescriptions(), Vk::PerInstanceVertex::GetAttributeDescriptions());

	scene.pipeline = new Vk::Pipeline(
		vs_code.GetContent(), fs_code.GetContent(), 
		ExtentToVec2(Vk::Global::swapChain->GetExtent()), Vk::Global::swapChain->GetImageFormat(), 
		vertex_buffer_binding_descriptors, vertex_buffer_attribute_descriptors,
		{ descriptorSetLayout->GetVkDescriptorSetLayout() }
	);
	scene.pipeline->SetAsOutput();

	{
		scene.dynamicVertexBuffer = new Vk::Buffer(sizeof(Vk::PerInstanceVertex), 16080, nullptr, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
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
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 30 }
	});

	scene.ubo.perScene.buffer = new Vk::Buffer(
		sizeof(UBOScene),
		1,
		&scene.ubo.perScene.data,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	);

	descriptorSet = new Vk::DescriptorSet(descriptorPool, { descriptorSetLayout->GetVkDescriptorSetLayout() });

	std::vector<VkWriteDescriptorSet> write_descriptor_sets = 
	{
		Vk::CreateWriteDescriptorSet(descriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &scene.ubo.perScene.buffer->GetDescriptor()),
		Vk::CreateWriteDescriptorSet(descriptorSet, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &imageView->GetDescriptor())
	};

	descriptorSet->Update(write_descriptor_sets);

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());

	game = new Game();
	
	/*
	* It's definitely efficient to write to command buffer once.
	* 
	* But I don't know if it's going to be possible in the conditions of the game.
	*
	* Note: Alternatively I can create command buffers (three) especially for this task (to render the map),
	* which I most likely will end up doing
	*/
	for (int i = 0; i < commandBuffers.size(); i++)
	{
		Vk::CommandBuffer* cmd = commandBuffers[i];
		Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];

		cmd->Begin();
			cmd->BeginRenderPass(scene.pipeline->GetRenderPass(), framebuffer);
				cmd->BindPipeline(scene.pipeline);
					cmd->BindVertexBuffers({ scene.mesh.vertexBuffer, scene.dynamicVertexBuffer }, { 0, 0 });
					cmd->BindIndexBuffer(scene.mesh.indexBuffer);
						cmd->BindDescriptorSets(scene.pipeline, 1, &descriptorSet->GetVkDescriptorSet());
						cmd->DrawIndexed(scene.mesh.indexBuffer->GetAmountOfElements(), game->map.GetAmountOfBlocks() /* TODO: Pre-calculate this! */, 0, 0, 0);
			cmd->EndRenderPass();
		cmd->End();
	}
}

void NaturaForge::UpdateMap()
{
	if (game->camera.GetEvents() & CameraEvents_PositionChanged)
	{
		game->map.CalculateVisibleBlocks(game->camera.GetPosition());
		if (!(game->map.lastVisibleBlocks.start == game->map.visibleBlocks.start && game->map.lastVisibleBlocks.end == game->map.visibleBlocks.end))
		{
			game->map.PopulateBlocks(game->camera.GetPosition());

			renderData = MapRenderer::GetRenderData(&game->map, game->camera.GetPosition());
			scene.dynamicVertexBuffer->Update(renderData.data(), static_cast<uint32_t>(sizeof(glm::vec4) * renderData.size()));
		}
	}
}

void NaturaForge::UpdateProjectionViewMatrix()
{
	static float speed = 15.0f;

	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_W))
	{
		game->camera.AddPosition(glm::vec2(0, -100) * Time::deltaTime * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_S))
	{
		game->camera.AddPosition(glm::vec2(0, 100) * Time::deltaTime * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_D))
	{
		game->camera.AddPosition(glm::vec2(100, 0) * Time::deltaTime * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_A))
	{
		game->camera.AddPosition(glm::vec2(-100, 0) * Time::deltaTime * speed);
	}

	if (game->camera.GetEvents() & CameraEvents_PositionChanged)
	{
		glm::vec2 half_size = window->GetSize() / 2.0f;
		glm::mat4x4 projection_matrix = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y);
		glm::mat4x4 view_matrix = glm::inverse(glm::translate(glm::mat4x4(1), glm::vec3(game->camera.GetPosition(), 0.0f)));
		glm::mat4x4 projection_view_matrix = projection_matrix * view_matrix;
		scene.ubo.perScene.buffer->Update(&projection_view_matrix);
	}
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

	game->camera.CheckPositionChange();
	UpdateProjectionViewMatrix();
	UpdateMap();
	Render(current_command_buffer);
	Present();

	static float timer = 0.0f;
	timer += Time::deltaTime;
	
	if (timer >= 1.0f)
	{
		LOG_OUT("Average FPS: {0}", Time::GetAverageFPS());
		timer = 0.0f;
	}
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();

	delete imageView;
	delete image;

	delete descriptorSet;
	delete descriptorPool;
	delete descriptorSetLayout;

	delete scene.ubo.perScene.buffer;
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