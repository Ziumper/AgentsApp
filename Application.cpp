#include "Application.h"

#include "imgui.h"
#include "Rae.h"
#include <string>
#include <stdio.h>
#include <time.h>

#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>
#include <implot.h>

using namespace OpenXLSX;

namespace AgentsApp
{
	static AgentsAppLog logger;

	AgentsAppLog::AgentsAppLog()
	{
		AutoScroll = true;
		Buf = ImGuiTextBuffer();
		Filter = ImGuiTextFilter();
		LineOffsets = ImVector<int>();
		Clear();
	}

	void AgentsAppLog::Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void AgentsAppLog::AddLogEntry(const char* fmt, ...)
	{
		if (Enabled == false) {
			return;
		}

		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void AgentsAppLog::AddLog(const char* logEntry) {
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		localtime_s(&tstruct, &now);
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
		// for more information about date/time format
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
		std::string entry = buf;
		entry.append(" ");
		entry.append(logEntry);
		entry.append("\n");

		AddLogEntry(entry.c_str());
	}

	void AgentsAppLog::AddLog(const char* logEntry,const int Number)
	{
		std::string entry = logEntry;
		auto numberAsString = std::to_string(Number);
		entry.append(numberAsString);
		AddLog(entry.c_str());
	}

	void AgentsAppLog::AddLog(const char* logEntry, const double Number)
	{
		std::string entry = logEntry;
		auto numberAsString = std::to_string(Number);
		entry.append(numberAsString);
		AddLog(entry.c_str());
	}

	void AgentsAppLog::Draw(const char* title, bool* p_open)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have random access to the result of our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of
				// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
				// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
				// within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
				// on your side is recommended. Using ImGuiListClipper requires
				// - A) random access into your data
				// - B) items all being the  same height,
				// both of which we can handle since we have an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display
				// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
				// it possible (and would be recommended if you want to search through tens of thousands of entries).
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}

	void RenderUI()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		logger.Draw("Log Window");
		ShowRtbsmWindow();

	
		//ImGui::ShowDemoWindow();
	}

	void ShowRtbsmWindow()
	{
		static Rae::MonteCarlo monteCarlo(&logger);
		ImGui::Begin("RTBS System");

		

		ImGui::SeparatorText("Inputs");

		ImGui::InputInt("Cycles", &monteCarlo.cyclesAmount);
		ImGui::InputInt("Agents", &monteCarlo.agentsAmount);
		ImGui::InputInt("s-Agents", &monteCarlo.strategicAgentsAmount);

		ImGui::InputInt("kMin", &monteCarlo.kMin);
		ImGui::InputInt("kMax", &monteCarlo.kMax);
		ImGui::InputDouble("expoA", &monteCarlo.expoA);
		ImGui::InputDouble("expoG", &monteCarlo.expoG);
		ImGui::SeparatorText("Good Will");
		ImGui::InputDouble("x", &monteCarlo.goodWill.x);
		ImGui::InputDouble("y", &monteCarlo.goodWill.y);
		ImGui::InputDouble("z", &monteCarlo.goodWill.z);
		ImGui::SeparatorText("Starting trust measure");
		ImGui::InputDouble("V_0 trust", &monteCarlo.beginTrustMesaure);
		ImGui::SeparatorText("Options");
		ImGui::Checkbox("Boost Mode", &monteCarlo.boostMode);
		ImGui::Checkbox("Logging On/Off", &logger.Enabled);

		ImGui::SeparatorText("Actions");

		if (monteCarlo.Done) {
			bool clicked = ImGui::Button("Start Again");
			if (clicked) {
				monteCarlo.Start();
			}

			bool exportFile = ImGui::Button("Export file");
			if (exportFile) {
				Export(monteCarlo);
			}
		}
		else {
			bool clicked = ImGui::Button("Start");
			if (clicked) {
				monteCarlo.Start();
			}
		}

		bool open = true;
		if (ImGui::BeginPopupModal("Export result", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
			std::string message = "Results exported into file:";
			message.append(" ./Report.xlsx");
			ImGui::Text(message.c_str());
			ImGui::EndPopup();
		}
		

		monteCarlo.Update();


		ImGui::End();

		ImGui::Begin("ViewPort");

		static float progress = 0.0f;
		if (monteCarlo.IsWorking()) {
			ImGui::Text("Work in progress...");
			int currentRecipientCount = monteCarlo.CurrentRecipientNumberInCycle();
			progress = (float)currentRecipientCount / (float)monteCarlo.agentsAmount;
		}
		else {
			ImGui::Text("Ready to start ..");
		}

		// Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
		// or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
		ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		std::string message = "Cycle: ";
		message.append(std::to_string(monteCarlo.CurrentCycleNumber()));
		ImGui::Text(message.c_str());
		
		if (monteCarlo.Done) {

			ImGui::SeparatorText("Plots");

			//Strategic plot draw
			if (ImPlot::BeginPlot("Strategic Agent Trust Traectory")) {
				ImPlot::PlotLine("Vs(t)", &monteCarlo.CycleNumbersForPlot[0], &monteCarlo.SAgentTraectoryAvg[0], monteCarlo.cyclesAmount);
				
				ImPlot::EndPlot();
			}

			//honest plot draw
			if (ImPlot::BeginPlot("Honest Agent Trust Traectory")) {
				ImPlot::PlotLine("Vh(t)", &monteCarlo.CycleNumbersForPlot[0], &monteCarlo.HAgentTraectoryAvg[0], monteCarlo.cyclesAmount);

				ImPlot::EndPlot();
			}

			ImGui::Separator();

			
			//netto outflow
			if (ImPlot::BeginPlot("Netto outflow from Honest agents")) {
				ImPlot::PlotLine("Vh(t)", &monteCarlo.CycleNumbersForPlot[0], &monteCarlo.NettoOutflow[0], monteCarlo.cyclesAmount);
				ImPlot::EndPlot();
			}

		}


		ImGui::End();
	}

	void Export(Rae::MonteCarlo simulation) {
		std::string fileName = "./Report.xlsx";

		XLDocument doc;
		doc.create(fileName);

		//create a new worksheet
		std::string worksheetName = "Cycles data";
		auto workbook = doc.workbook();
		workbook.addWorksheet(worksheetName);
		auto cyclesWorkSheet = workbook.worksheet(worksheetName);
		
		//delete first one which is default one
		std::string defaultWorkSheetName = "Sheet1";
		workbook.deleteSheet(defaultWorkSheetName);

		//set column names 
		
		cyclesWorkSheet.cell("A1").value() = "Cycle";
		cyclesWorkSheet.cell("B1").value() = "S Traectory"; 
		cyclesWorkSheet.cell("C1").value() = "H Traectory";
		cyclesWorkSheet.cell("D1").value() = "Netto";

		//set cycles number
		std::string columnLetter = "A";
		for (int i = 0; i < simulation.cyclesAmount; i++) {
			std::string myCellCordinate = columnLetter;
			int columnNumber = i + 2;
			myCellCordinate.append(std::to_string(columnNumber));
			XLCell cell = cyclesWorkSheet.cell(myCellCordinate);
			int valueToSave = i + 1;
			cell.value() = valueToSave;
		}

		//save values for first cell
		columnLetter = "B";
		for (int i = 0; i < simulation.cyclesAmount; i++) {
			std::string myCellCordinate = columnLetter;
			int columnNumber = i + 2;
			myCellCordinate.append(std::to_string(columnNumber));
			XLCell cell = cyclesWorkSheet.cell(myCellCordinate);
			float valueToSave = simulation.SAgentTraectoryAvg[i];
			cell.value() = valueToSave;
		}

		//save values for second cell
		columnLetter = "C";
		for (int i = 0; i < simulation.cyclesAmount; i++) {
			std::string myCellCordinate = columnLetter;
			int columnNumber = i + 2;
			myCellCordinate.append(std::to_string(columnNumber));
			XLCell cell = cyclesWorkSheet.cell(myCellCordinate);
			float valueToSave = simulation.HAgentTraectoryAvg[i];
			cell.value() = valueToSave;
		}

		//save values for second cell
		columnLetter = "D";
		for (int i = 0; i < simulation.cyclesAmount; i++) {
			std::string myCellCordinate = columnLetter;
			int columnNumber = i + 2;
			myCellCordinate.append(std::to_string(columnNumber));
			XLCell cell = cyclesWorkSheet.cell(myCellCordinate);
			float valueToSave = simulation.NettoOutflow[i];
			cell.value() = valueToSave;
		}
		
		doc.save();
		doc.close();
		
		ImGui::OpenPopup("Export result");

	}


}