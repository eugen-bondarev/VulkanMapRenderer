#include "text_editor.h"

#include "imgui/imgui.h"
// #include "imgui/imgui_impl_vulkan.h"

#include <vector>

namespace Programs
{
	namespace TextEditor
	{
		struct
		{
			std::string buffer = "";
		} state;

		void Init()
		{
			state.buffer.resize(10000);
		}

		void Render()
		{
			ImGui::Begin("Text editor");
				ImGui::BeginChild(1, ImVec2(800, 600));
					ImGui::InputTextMultiline("", state.buffer.data(), state.buffer.size(), ImVec2(700, 500));
				ImGui::EndChild();
			ImGui::End();
		}
	}
}