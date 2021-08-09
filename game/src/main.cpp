#include "main.h"

#include "ui/ui.h"
#include "sync/sync.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#include "programs/map_editor.h"
#include "programs/text_editor.h"

ImTextureID imGuiTextureID;

void NaturaForge::Init()
{
	state = std::make_shared<State>();

	gameFrameManager = new Vk::FrameManager(
		FrameSemaphore_FIRST_STAGE, 
		FrameSemaphore_LAST_STAGE, 
		FrameSemaphore_COUNT, 
		2
	);

	menuFrameManager = new Vk::FrameManager(
		MenuFrameSemaphore_FIRST_STAGE, 
		MenuFrameSemaphore_LAST_STAGE, 
		MenuFrameSemaphore_COUNT, 
		2
	);

	Programs::TextEditor::Init();
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	Vk::FrameManager* frame_manager = menu ? menuFrameManager : gameFrameManager;

	frame_manager->AcquireSwapChainImage();

	if (menu)
	{
		UI_START();
		
			Programs::TextEditor::Render();

		UI_END();
	}
	else
	{
		UI_START();
			ImGui::Begin("Info");
				ImGui::Text("FPS: %i", static_cast<int>(ImGui::GetIO().Framerate));
			ImGui::End();
			
			// Programs::MapEditor::Render();
			Programs::TextEditor::Render();
		UI_END();		
	}
	
	Collections::IOnUpdate::UpdateAll();
	Collections::IRenderable::RenderAll(frame_manager->GetCurrentFrame());

	frame_manager->Present();
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();
	delete menuFrameManager;
	delete gameFrameManager;
}

ENTRY()