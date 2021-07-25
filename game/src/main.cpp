#include "main.h"

#include "ui/ui.h"

ImTextureID userTextureID;

void NaturaForge::Init()
{
	game = std::make_shared<Game>();

	frameManager = new Vk::FrameManager();

	imagesInFlight.resize(Vk::Global::swapChain->GetImageViews().size());

	game->map->CalculateVisibleChunks(game->camera->GetPosition());
	game->map->PopulateBlocks();
	game->mapRenderer->Init(game->map, game->camera, Vk::Global::descriptorPool);
}

void NaturaForge::UpdateProjectionViewMatrix()
{
	static float speed = 1500.0f;
	// static float speed = 150.0f;

	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_W))
	{
		game->camera->AddPosition(glm::vec2(0, -1) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_S))
	{
		game->camera->AddPosition(glm::vec2(0, 1) * Time::GetDelta() * speed);
	}
	// if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_D))
	{
		game->camera->AddPosition(glm::vec2(1, 0) * Time::GetDelta() * speed);
	}
	if (glfwGetKey(window->GetGLFWWindow(), GLFW_KEY_A))
	{
		game->camera->AddPosition(glm::vec2(-1, 0) * Time::GetDelta() * speed);
	}

	if (game->camera->GetEvents() & CameraEvents_PositionChanged)
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
	game->camera->CheckPositionChange();
	UpdateProjectionViewMatrix();

	UI::AddToStack([&]()
	{
		ImGui::Begin("Info");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	});
	
	Collections::IOnUpdate::UpdateAll();
	Collections::IRenderable::RenderAll(frameManager->GetCurrentFrame());
	
	Present();
	
	frameManager->NextFrame();

	static float exit_timer = 0.0f;
	exit_timer += Time::GetDelta();

	if (exit_timer >= 5.0f)
	{
		glfwSetWindowShouldClose(window->GetGLFWWindow(), 1);
	}
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();

	delete frameManager;
}

ENTRY()