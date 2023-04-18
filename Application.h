#pragma once
#include <cstddef>
#include "imgui.h"

namespace AgentsApp
{
	class AgentsAppLog {
	private:
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.
	public:
		void Clear();
		void AddLog(const char* fmt, ...);
		void Draw(const char* title, bool* p_open = NULL);
		AgentsAppLog();
	};

	void RenderUI();
	void ShowRbtsmWindow();
}