#include "Application.h"

#include "imgui.h"
#include "Rae.h"

namespace AgentsApp
{

	void RenderUI()
	{
		ImGui::Begin("Settings");
		ImGui::Button("Hello");

		static float value = 0.0f;

		Rae::Reputation reputation = Rae::Reputation();

		reputation.setProvidedServices(1);
		reputation.setRequestedServices(2);

		int suppiler = reputation.getSuplierBehaviour();

		ImGui::Text("Testing text");

		ImGui::DragFloat("Value", &value);
		ImGui::End();
	}

}