#pragma once
#include <cstddef>
#include "imgui.h"
#include <string>

namespace AgentsApp
{
	class AgentsAppLog {
	private:
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.
		void AddLogEntry(const char* fmt, ...);
	public:
		void Clear();
		void AddLog(std::string logEntry);
		void Draw(const char* title, bool* p_open = NULL);
		AgentsAppLog();
	};

	void RenderUI();
	void ShowRbtsmWindow(AgentsAppLog *log);
}