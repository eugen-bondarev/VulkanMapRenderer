#include "main.h"

#include "renderer/world/map/map_renderer.h"

void NaturaForge::Init()
{
	game = std::make_unique<Game>();

	frameManager = new Vk::FrameManager();

	// Create command buffers for each swap chain image.
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
		commandBuffers.push_back(new Vk::CommandBuffer(Vk::Global::commandPool));

	// Load image
	{
		const Assets::Image map_texture("assets/textures/map.png");

		tileMap = TextureAtlas::Add<BlocksTileMap>(TextureAtlasType::Map, std::make_shared<BlocksTileMap>(
			glm::vec2(8.0f),
			map_texture.GetSize(),
			map_texture.GetAmountOfChannels(),
			map_texture.GetData()
		));

		tileMap->Add(BlockType::Dirt, glm::vec2(1, 1));
		tileMap->Add(BlockType::Grass, glm::vec2(1, 7));
		tileMap->Add(BlockType::Stone, glm::vec2(7, 1));
		tileMap->Add(BlockType::Wood, glm::vec2(13, 1));
	}

	// Creating descriptor set layout (for the pipeline)
	std::vector<VkDescriptorSetLayoutBinding> bindings = 
	{
		Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
		Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayout = new Vk::DescriptorSetLayout(bindings);

	// Creating pipeline
	const Assets::Text vs_code("assets/shaders/default.vert.spv");
	const Assets::Text fs_code("assets/shaders/default.frag.spv");

	Vk::BindingDescriptions vertex_buffer_binding_descriptors = Util::Vector::Merge(Vk::Vertex::GetBindingDescriptions(), Vk::PerInstanceVertex::GetBindingDescriptions());
	Vk::AttributeDescriptions vertex_buffer_attribute_descriptors = Util::Vector::Merge(Vk::Vertex::GetAttributeDescriptions(), Vk::PerInstanceVertex::GetAttributeDescriptions());

	scene.pipeline = new Vk::Pipeline(
		vs_code.GetContent(), fs_code.GetContent(), 
		ExtentToVec2(Vk::Global::swapChain->GetExtent()), Vk::Global::swapChain->GetImageFormat(), 
		vertex_buffer_binding_descriptors, vertex_buffer_attribute_descriptors,
		{ descriptorSetLayout->GetVkDescriptorSetLayout() }
	);

	// Swap chain's framebuffers will use its output
	scene.pipeline->SetAsOutput();

	// Dynamic buffer for blocks' positions
	scene.dynamicVertexBuffer = new Vk::Buffer(sizeof(Vk::PerInstanceVertex), game->map->GetAmountOfBlocks(), nullptr, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	// Loading the block's vertex & index buffers. I created a scope in order for it to free the staging buffer automatically.
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

	// Creating descriptor pool
	descriptorPool = new Vk::DescriptorPool({
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 30 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 30 }
	});

	// Creating a uniform buffer (Scene scope)
	scene.ubo.perScene.buffer = new Vk::Buffer(
		sizeof(UBOScene),
		1,
		&scene.ubo.perScene.data,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
	);

	// Creating descriptor set
	descriptorSet = new Vk::DescriptorSet(descriptorPool, { descriptorSetLayout->GetVkDescriptorSetLayout() });

	std::vector<VkWriteDescriptorSet> write_descriptor_sets = 
	{
		Vk::CreateWriteDescriptorSet(descriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &scene.ubo.perScene.buffer->GetDescriptor()),
		Vk::CreateWriteDescriptorSet(descriptorSet, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &tileMap->GetImageView()->GetDescriptor())
	};

	descriptorSet->Update(write_descriptor_sets);

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());
	
	/*
	* It's definitely efficient to write to the command buffer once.
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
						cmd->DrawIndexed(scene.mesh.indexBuffer->GetAmountOfElements(), game->map->GetAmountOfBlocks(), 0, 0, 0);
			cmd->EndRenderPass();
		cmd->End();
	}
}

void NaturaForge::UpdateMap()
{
	if (game->camera.GetEvents() & CameraEvents_PositionChanged)
	{
		game->map->CalculateVisibleBlocks(game->camera.GetPosition());
		if (!(game->map->lastVisibleBlocks.start == game->map->visibleBlocks.start && game->map->lastVisibleBlocks.end == game->map->visibleBlocks.end))
		{
			game->map->PopulateBlocks(game->camera.GetPosition());

			{
				std::vector<glm::vec4> render_data;
				MapRenderer::GetRenderData(game->map.get(), game->camera.GetPosition(), render_data);

				MW_PROFILER_NAMED_SCOPE("Update buffer");
				scene.dynamicVertexBuffer->Update(render_data.data(), static_cast<uint32_t>(sizeof(glm::vec4) * game->map->GetAmountOfBlocks()));
			}
		}
	}
}

void NaturaForge::UpdateProjectionViewMatrix()
{
	MW_PROFILER_SCOPE();

	static float speed = 15.0f;

	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_W))
	{
		game->camera.AddPosition(glm::vec2(0, -100) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_S))
	{
		game->camera.AddPosition(glm::vec2(0, 100) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_D))
	{
		game->camera.AddPosition(glm::vec2(100, 0) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_A))
	{
		game->camera.AddPosition(glm::vec2(-100, 0) * Time::GetDelta() * speed);
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
	MW_PROFILER_SCOPE();

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
	MW_PROFILER_SCOPE();
	Vk::Global::swapChain->Present(&frameManager->GetCurrentFrame()->GetRenderFinishedSemaphore(), 1);
	frameManager->NextFrame();
}

void NaturaForge::Update()
{
	MW_PROFILER_SCOPE();

	Vk::Frame* current_frame = frameManager->GetCurrentFrame();
	uint32_t image_index = Vk::Global::swapChain->AcquireImage(current_frame->GetImageAvailableSemaphore());

	if (imagesInFlight[image_index] != VK_NULL_HANDLE) 
		vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &imagesInFlight[image_index], VK_TRUE, UINT64_MAX);

	vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence(), VK_TRUE, UINT64_MAX);
	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence());

	imagesInFlight[image_index] = current_frame->GetInFlightFence();

	// Game logic
	game->camera.CheckPositionChange();
	UpdateProjectionViewMatrix();
	UpdateMap();

	// Renderer
	Vk::CommandBuffer* current_command_buffer = commandBuffers[image_index];

	Render(current_command_buffer);
	Present();

	static float exit_timer = 0.0f;
	exit_timer += Time::GetDelta();
	
	if (exit_timer >= 1.0f)
	{
		LOG_OUT("Average FPS: {0}", Time::GetAverageFPS());
		exit_timer = 0.0f;
		// glfwSetWindowShouldClose(window->GetGLFWWindow(), 1);
	}
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();
	
	tileMap.reset();

	delete descriptorSet;
	delete descriptorPool;
	delete descriptorSetLayout;

	delete scene.ubo.perScene.buffer;
	delete scene.mesh.vertexBuffer;
	delete scene.mesh.indexBuffer;
	delete scene.pipeline;

	delete scene.dynamicVertexBuffer;

	for (int i = 0; i < commandBuffers.size(); i++)
		delete commandBuffers[i];

	delete frameManager;
}

ENTRY()