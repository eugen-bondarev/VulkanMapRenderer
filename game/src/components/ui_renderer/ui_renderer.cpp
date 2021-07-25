#include "ui_renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

#include "ui/ui.h"

using namespace Engine;

UIRenderer::UIRenderer()
{
	renderPass = new Vk::RenderPass(
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

	vkDeviceWaitIdle(Vk::Global::device->GetVkDevice());
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	// Create command buffers for each swap chain image.
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* commandPool = new Vk::CommandPool();
		commandBuffers.push_back(new Vk::CommandBuffer(commandPool));
		commandPools.push_back(commandPool);
	}
}

UIRenderer::~UIRenderer()
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

void UIRenderer::Render(Engine::Vk::Frame* frame)
{
	VT_PROFILER_SCOPE();
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	UI::ExecuteStack();
	UI::ClearStack();

	ImGui::Render();

	Vk::CommandPool* pool = commandPools[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::CommandBuffer* cmd = commandBuffers[Vk::Global::swapChain->GetCurrentImageIndex()];
	Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetCurrentScreenFramebuffer();

	VkSemaphore* wait = &frame->GetRenderFinishedSemaphore();
	VkSemaphore* signal = &frame->GetImGuiRenderFinishedSemaphore();
	
	pool->Reset();
	cmd->Begin();
		cmd->BeginRenderPass(renderPass, framebuffer);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd->GetVkCommandBuffer());
		cmd->EndRenderPass();
	cmd->End();

	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal);
}