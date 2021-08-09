#include "map_editor.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#include <vector>

namespace Programs
{
	namespace MapEditor
	{
		using ColumnOfTiles_t = std::vector<glm::vec2>;
		using Tiles_t = std::vector<ColumnOfTiles_t>;

		struct 
		{
			ImTextureID imGuiTexture;
			Tiles_t tiles;
		} state;

		static const glm::ivec2 MAIN_WINDOW_DIMENSIONS = glm::ivec2 { 5, 5 };
		static constexpr float MAIN_WINDOW_TILE_SIZE_P = { 64 };
		static constexpr float POPUP_WINDOW_TILE_SIZE_P = { 32 };

		static InitData initData;

		void Init(InitData init_data)
		{
			initData = init_data;

			state.imGuiTexture = ImGui_ImplVulkan_AddTexture(
				Engine::Vk::Global::constantInterpolationSampler->GetVkSampler(),
				initData.texture->GetImageView()->GetVkImageView(), 
				initData.texture->GetImageView()->GetDescriptor().imageLayout 
			);
			
			state.tiles.resize(MAIN_WINDOW_DIMENSIONS.x);

			for (int i = 0; i < state.tiles.size(); i++)
			{
				state.tiles[i].resize(MAIN_WINDOW_DIMENSIONS.y);

				for (int j = 0; j < state.tiles[i].size(); j++)
				{
					state.tiles[i][j] = glm::vec2 { 2, 4 };
				}
			}
		}

		void Render()
		{
			ImGui::SetNextWindowSize(ImVec2(
				MAIN_WINDOW_TILE_SIZE_P * state.tiles.size() + ImGui::GetStyle().WindowPadding.x * 2.0f, 
				MAIN_WINDOW_TILE_SIZE_P * state.tiles[0].size() + ImGui::GetStyle().WindowPadding.y * 2.0f + 20.0f)
			);

			ImGui::Begin("Blocks", nullptr, ImGuiWindowFlags_NoResize);				
				glm::vec2 initial_cursor_pos = { ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };

				int id = 0;

				for (int i = 0; i < state.tiles.size(); i++)
				{
					for (int j = 0; j < state.tiles[i].size(); j++)
					{
						glm::vec2 uv0 = (initData.tileSize / initData.textureSize) * state.tiles[i][j];
						glm::vec2 uv1 = (initData.tileSize / initData.textureSize) * (state.tiles[i][j] + 1.0f);

						ImGui::SetCursorPos(ImVec2(initial_cursor_pos.x + i * MAIN_WINDOW_TILE_SIZE_P, initial_cursor_pos.y + j * MAIN_WINDOW_TILE_SIZE_P));

						ImGui::PushID(id++);

						if (ImGui::ImageButton(state.imGuiTexture, ImVec2(MAIN_WINDOW_TILE_SIZE_P, MAIN_WINDOW_TILE_SIZE_P), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), 0))
						{
							ImGui::OpenPopup("my_select_popup");
						}

						if (ImGui::BeginPopup("my_select_popup"))
						{
							glm::vec2 initial_cursor_pos_popup = { ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };

							int id_0 = state.tiles.size() * state.tiles[0].size();

							for (int x = 0; x < initData.textureSize.x / initData.tileSize; x++)
							{
								for (int y = 0; y < initData.textureSize.y / initData.tileSize; y++)
								{
									glm::vec2 u0 = (initData.tileSize / initData.textureSize) * glm::vec2(x, y);
									glm::vec2 u1 = (initData.tileSize / initData.textureSize) * (glm::vec2(x, y) + 1.0f);

									ImGui::SetCursorPos(ImVec2(initial_cursor_pos_popup.x + x * POPUP_WINDOW_TILE_SIZE_P, initial_cursor_pos_popup.y + y * POPUP_WINDOW_TILE_SIZE_P));

									ImGui::PushID(id_0++);
									if (ImGui::ImageButton(state.imGuiTexture, ImVec2(POPUP_WINDOW_TILE_SIZE_P, POPUP_WINDOW_TILE_SIZE_P), ImVec2(u0.x, u0.y), ImVec2(u1.x, u1.y), 0))
									{
										state.tiles[i][j] = glm::vec2 { x, y };
										ImGui::CloseCurrentPopup();
									}
									ImGui::PopID();
								}
							}
							
							ImGui::EndPopup();
						}

						ImGui::PopID();
					}
				}
			ImGui::End();
		}
	}
}