#include "main.h"

#include "ui/ui.h"
#include "sync/sync.h"
#include "imgui/imgui.h"

void NaturaForge::Init()
{
	state = std::make_shared<State>();
	frameManager = new Vk::FrameManager(FrameSemaphore_FIRST_STAGE, FrameSemaphore_LAST_STAGE, FrameSemaphore_COUNT, 2);
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	frameManager->AcquireSwapChainImage();

		UI_START();

		ImGui::Begin("Info");
			ImGui::Text("FPS: %i", static_cast<int>(ImGui::GetIO().Framerate));
		ImGui::End();

		UI_END();
		
		Collections::IOnUpdate::UpdateAll();
		Collections::IRenderable::RenderAll(frameManager->GetCurrentFrame());
	
	frameManager->Present();
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();
	delete frameManager;
}

ENTRY()