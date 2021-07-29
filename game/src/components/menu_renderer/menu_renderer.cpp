#include "menu_renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "ui/ui.h"
#include "sync/sync.h"

using namespace Engine;

MenuRenderer::MenuRenderer()
{
	renderPass = new Vk::RenderPass(
		{ Vk::Util::CreateAttachment(
			Vk::Global::swapChain->GetImageFormat(), 

			// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
			VK_IMAGE_LAYOUT_UNDEFINED, 

			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 

			// VK_ATTACHMENT_LOAD_OP_LOAD, 
			VK_ATTACHMENT_LOAD_OP_CLEAR, 

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
	init_info.DescriptorPool = Vk::Global::descriptorPool->GetVkDescriptorPool();
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info, renderPass->GetVkRenderPass());

	Vk::CommandBuffer my_command_buffer(Vk::Global::commandPool);

	my_command_buffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		ImGui_ImplVulkan_CreateFontsTexture(my_command_buffer.GetVkCommandBuffer());
	my_command_buffer.End();

	my_command_buffer.SubmitToQueue(Vk::Global::Queues::graphicsQueue);

	Vk::Global::device->WaitIdle();
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* commandPool = new Vk::CommandPool();
		commandPools.push_back(commandPool);
		commandBuffers.push_back(new Vk::CommandBuffer(commandPool));
	}

	Vk::Global::swapChain->InitFramebuffers(renderPass->GetVkRenderPass());
}

MenuRenderer::~MenuRenderer()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (auto& buffer : commandBuffers)
		delete buffer;

	for (auto& pool : commandPools)
		delete pool;

	delete renderPass;	
}

void MenuRenderer::Render(Engine::Vk::Frame* frame)
{
	VT_PROFILER_SCOPE();

	// VkSemaphore* wait = &frame->GetSemaphore(FrameSemaphore_MapRenderFinished);
	// VkSemaphore* signal = &frame->GetSemaphore(FrameSemaphore_ImGuiRenderFinished);

	VkSemaphore* wait = &frame->GetSemaphore(MenuFrameSemaphore_ImageAvailable);
	VkSemaphore* signal = &frame->GetSemaphore(MenuFrameSemaphore_ImGuiRenderFinished);

	Vk::CommandPool* pool = commandPools[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::CommandBuffer* cmd = commandBuffers[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetCurrentScreenFramebuffer();
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		UI::ExecuteStack();
		UI::ClearStack();
	ImGui::Render();
	
	pool->Reset();
		cmd->Begin();
			cmd->BeginRenderPass(renderPass, framebuffer);
				ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd->GetVkCommandBuffer());
			cmd->EndRenderPass();
		cmd->End();

	VkFence fence = frame->GetInFlightFence();

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &fence);
	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal, fence);
}