#include "main.h"

#include "ui/ui.h"
#include "sync/sync.h"
#include "imgui/imgui.h"

void NaturaForge::Init()
{
	game = std::make_shared<Game>();
	frameManager = new Vk::FrameManager(FrameSemaphore_COUNT, 2);
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	frameManager->AcquireSwapChainImage(FrameSemaphore_FIRST_STAGE);

	UI_PUSH(
		ImGui::Begin("Info");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	);
	
	Collections::IOnUpdate::UpdateAll();
	Collections::IRenderable::RenderAll(frameManager->GetCurrentFrame());
	
	frameManager->Present(FrameSemaphore_LAST_STAGE);
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();
	delete frameManager;
}

ENTRY()