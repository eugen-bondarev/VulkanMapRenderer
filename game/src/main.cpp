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
}

void NaturaForge::Update()
{
	VT_PROFILER_SCOPE();

	Vk::FrameManager* frame_manager = menu ? menuFrameManager : gameFrameManager;

	frame_manager->AcquireSwapChainImage();

	if (menu)
	{
		UI_START();
			ImGui::Begin("Menu");
				if (ImGui::Button("Start game") || true)
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

						// auto& tile_map_texture = state->mapRenderer->colorPass->block.tileMap;

						// imGuiTextureID = ImGui_ImplVulkan_AddTexture(
						// 	Vk::Global::constantInterpolationSampler->GetVkSampler(),
						// 	tile_map_texture->GetImageView()->GetVkImageView(), 
						// 	tile_map_texture->GetImageView()->GetDescriptor().imageLayout 
						// );

						// Programs::MapEditor::InitData init_data;
						// BlocksTileMap* texture_2d = state->mapRenderer->colorPass->block.tileMap.get();
						// init_data.texture = texture_2d;
						// init_data.textureSize = texture_2d->GetSize();
						// init_data.tileSize = texture_2d->GetTileSize().x;
						
						// Programs::MapEditor::Init(init_data);
						Programs::TextEditor::Init();

						menu = false;
					VT_TASK_END();
				}
				if (ImGui::Button("Exit"))
				{
					window->Close();
				}
			ImGui::End();
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