#include "main.h"

#include <future>

#include "ui/ui.h"

void NaturaForge::InitCommonResources()
{
	common.descriptorPool = new Vk::DescriptorPool({		
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	});
}

void NaturaForge::InitImGui()
{
	imgui.renderPass = new Vk::RenderPass(
		{ Vk::Util::CreateAttachment(
			Vk::Global::swapChain->GetImageFormat(), 
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
			VK_ATTACHMENT_LOAD_OP_LOAD, 
			VK_ATTACHMENT_STORE_OP_STORE
		) }
	);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindow(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = Vk::Global::instance->GetVkInstance();
	init_info.PhysicalDevice = Vk::Global::device->GetVkPhysicalDevice();
	init_info.Device = Vk::Global::device->GetVkDevice();
	init_info.QueueFamily = Vk::Global::Queues::indices.graphicsFamily.value();
	init_info.Queue = Vk::Global::Queues::graphicsQueue;
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = common.descriptorPool->GetVkDescriptorPool();
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info, imgui.renderPass->GetVkRenderPass());

	Vk::CommandBuffer my_command_buffer(Vk::Global::commandPool);

	my_command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		ImGui_ImplVulkan_CreateFontsTexture(my_command_buffer.GetVkCommandBuffer());
	my_command_buffer.End();

	my_command_buffer.SubmitToQueue(Vk::Global::Queues::graphicsQueue);

	vkDeviceWaitIdle(Vk::Global::device->GetVkDevice());
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	// Create command buffers for each swap chain image.
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* commandPool = new Vk::CommandPool();
		imgui.commandBuffers.push_back(new Vk::CommandBuffer(commandPool));
		imgui.commandPools.push_back(commandPool);
	}
}

ImTextureID userTextureID;

void NaturaForge::Init()
{
	game = std::make_shared<Game>();

	frameManager = new Vk::FrameManager();

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());

	InitCommonResources();

	game->mapRenderer->Init(game->map, &game->camera, common.descriptorPool);

	// mapRenderer = new MapRenderer(common.descriptorPool, game);	
	game->mapRenderer->FillCommandBuffers();

	InitImGui();
}

void NaturaForge::UpdateMap()
{
	if (game->camera.GetEvents() & CameraEvents_PositionChanged)
	{
		game->map->CalculateVisibleChunks(game->camera.GetPosition());
		if (game->map->lastVisibleChunks.start != game->map->visibleChunks.start || game->map->lastVisibleChunks.end != game->map->visibleChunks.end)
		{
			game->map->PopulateBlocks();
			game->mapRenderer->Update();

			game->map->lastVisibleChunks = game->map->visibleChunks;
		}
	}
}

void NaturaForge::UpdateProjectionViewMatrix()
{
	VT_PROFILER_SCOPE();

	static float speed = 1500.0f;

	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_W))
	{
		game->camera.AddPosition(glm::vec2(0, -1) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_S))
	{
		game->camera.AddPosition(glm::vec2(0, 1) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_D))
	{
		game->camera.AddPosition(glm::vec2(1, 0) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_A))
	{
		game->camera.AddPosition(glm::vec2(-1, 0) * Time::GetDelta() * speed);
	}

	if (game->camera.GetEvents() & CameraEvents_PositionChanged)
	{
		game->mapRenderer->UpdateSpace();
	}
}

void NaturaForge::Render(Vk::CommandBuffer* cmd)
{	
	VT_PROFILER_SCOPE();

	Vk::Frame* current_frame = frameManager->GetCurrentFrame();

	VkSemaphore* wait = &current_frame->GetImageAvailableSemaphore();
	VkSemaphore* signal = &current_frame->GetRenderFinishedSemaphore();
	VkFence fence = current_frame->GetInFlightFence();

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &fence);
	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal, fence);
}

void NaturaForge::FillImGuiCommandBuffers()
{
	VT_PROFILER_SCOPE();

	Vk::CommandPool* pool = imgui.commandPools[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::CommandBuffer* cmd = imgui.commandBuffers[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetCurrentScreenFramebuffer();

	Vk::Frame* current_frame = frameManager->GetCurrentFrame();
	VkSemaphore* wait = &current_frame->GetRenderFinishedSemaphore();
	VkSemaphore* signal = &current_frame->GetImGuiRenderFinishedSemaphore();
	
	pool->Reset();
	cmd->Begin();
		cmd->BeginRenderPass(imgui.renderPass, framebuffer);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd->GetVkCommandBuffer());
		cmd->EndRenderPass();
	cmd->End();

	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal);
}

void NaturaForge::RenderUI()
{	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	UI::ExecuteStack();
	UI::ClearStack();

	ImGui::Begin("Info");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Render();
}

void NaturaForge::Present()
{
	VT_PROFILER_SCOPE();

	Vk::Global::swapChain->Present(&frameManager->GetCurrentFrame()->GetImGuiRenderFinishedSemaphore(), 1);
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	Vk::Frame* current_frame = frameManager->GetCurrentFrame();
	uint32_t image_index = Vk::Global::swapChain->AcquireImage(current_frame->GetImageAvailableSemaphore());

	if (imagesInFlight[image_index] != VK_NULL_HANDLE)
	{
		vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &imagesInFlight[image_index], VK_TRUE, UINT64_MAX);
	} 

	vkWaitForFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence(), VK_TRUE, UINT64_MAX);
	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &current_frame->GetInFlightFence());

	imagesInFlight[image_index] = current_frame->GetInFlightFence();

	// Game logic
	game->camera.CheckPositionChange();
	UpdateProjectionViewMatrix();
	UpdateMap();

	RenderUI();
	
	game->mapRenderer->Render(frameManager->GetCurrentFrame());
	FillImGuiCommandBuffers();
	
	Present();
	
	frameManager->NextFrame();
}

void NaturaForge::ShutdownImGui()
{	
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (auto& buffer : imgui.commandBuffers)
		delete buffer;

	for (auto& pool : imgui.commandPools)
		delete pool;

	delete imgui.renderPass;
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();

	ShutdownImGui();

	delete common.descriptorPool;

	// delete game->MAP_RENDERER;

	delete frameManager;
}

ENTRY()