#include "main.h"

#include "ui/ui.h"
#include "sync/sync.h"
#include "imgui/imgui.h"

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
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	if (menu)
	{
		menuFrameManager->AcquireSwapChainImage();

			UI_START();

			ImGui::Begin("Menu");
				if (ImGui::Button("Start game"))
				{
					VT_TASK_START();
						Vk::Global::device->WaitIdle();

						delete state->menuEntity;
						
						state->mapEntity = new Entity();
						state->uiEntity = new Entity();
						
						state->camera = state->mapEntity->AddComponent<Camera>();
						state->map = state->mapEntity->AddComponent<Map>(state->camera);
						state->mapRenderer = state->mapEntity->AddComponent<MapRenderer>(state->camera, state->map);

						state->uiRenderer = state->uiEntity->AddComponent<UIRenderer>();

						menu = false;
					VT_TASK_END();
				}
				if (ImGui::Button("Exit"))
				{
					window->Close();
				}
			ImGui::End();

			UI_END();
			
			Collections::IOnUpdate::UpdateAll();
			Collections::IRenderable::RenderAll(menuFrameManager->GetCurrentFrame());
		
		menuFrameManager->Present();
	}
	else
	{
		gameFrameManager->AcquireSwapChainImage();

			UI_START();

			ImGui::Begin("Info");
				ImGui::Text("FPS: %i", static_cast<int>(ImGui::GetIO().Framerate));
			ImGui::End();

			UI_END();
			
			Collections::IOnUpdate::UpdateAll();
			Collections::IRenderable::RenderAll(gameFrameManager->GetCurrentFrame());
		
		gameFrameManager->Present();
	}
}

void NaturaForge::Shutdown()
{
	Vk::Global::device->WaitIdle();
	delete menuFrameManager;
	delete gameFrameManager;
}

ENTRY()