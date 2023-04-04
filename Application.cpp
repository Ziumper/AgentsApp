#include "Application.h"

#include "imgui.h"

namespace AgentsApp
{

	void RenderUI()
	{
		ImGui::Begin("Settings");
		ImGui::Button("Hello");

		static float value = 0.0f;

		ImGui::DragFloat("Value", &value);
		ImGui::End();
	}

}