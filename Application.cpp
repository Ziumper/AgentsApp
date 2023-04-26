#include "Application.h"

#include "imgui.h"
#include "Rae.h"
#include <string>
#include <stdio.h>
#include <time.h>

#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>

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
		}
		else {
			bool clicked = ImGui::Button("Start");
			if (clicked) {
				monteCarlo.Start();
			}
		}
		

		monteCarlo.Update();

		bool exportFile = ImGui::Button("Export file");
		if (exportFile) {
			Export();
		}

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
			static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
			ImGui::PlotLines("Vs(t) Traectory ", &monteCarlo.SAgentTraectoryAvg[0], monteCarlo.cyclesAmount, 0, NULL, 0, 1.0f, ImVec2(0, 80));
			ImGui::PlotLines("Vh(t) Traectory ", &monteCarlo.HAgentTraectoryAvg[0], monteCarlo.cyclesAmount, 0, NULL, 0, 1.0f, ImVec2(0, 80));
			ImGui::PlotLines("Netto Outflow", &monteCarlo.NettoOutflow[0], monteCarlo.cyclesAmount, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));
			ImGui::PlotLines("Final Trust", &monteCarlo.FinalTrust[0], monteCarlo.agentsAmount, 0, NULL, 0, 1.0f, ImVec2(0, 80));
		}

		//std::vector<float> trustLevels = monteCarlo.GetTrustLevels();
		

		

		ImGui::End();
	}

	void Export() {

		// This example program illustrates basic usage of OpenXLSX, for example creation of a new workbook, and read/write
		// of cell values.

		// First, create a new document and access the sheet named 'Sheet1'.
		// New documents contain a single worksheet named 'Sheet1'
		XLDocument doc;
		doc.create("./Demo01.xlsx");
		auto wks = doc.workbook().worksheet("Sheet1");

		// The individual cells can be accessed by using the .cell() method on the worksheet object.
		// The .cell() method can take the cell address as a string, or alternatively take a XLCellReference
		// object. By using an XLCellReference object, the cells can be accessed by row/column coordinates.
		// The .cell() method returns an XLCell object.

		// The .value() method of an XLCell object can be used for both getting and setting the cell value.
		// Setting the value of a cell can be done by using the assignment operator on the .value() method
		// as shown below. Alternatively, a .set() can be used. The cell values can be floating point numbers,
		// integers, strings, and booleans. It can also accept XLDateTime objects, but this requires special
		// handling (see later).
		wks.cell("A1").value() = 3.14159265358979323846;
		wks.cell("B1").value() = 42;
		wks.cell("C1").value() = "  Hello OpenXLSX!  ";
		wks.cell("D1").value() = true;
		wks.cell("E1").value() = std::sqrt(-2); // Result is NAN, resulting in an error value in the Excel spreadsheet.

		// As mentioned, the .value() method can also be used for getting tha value of a cell.
		// The .value() method returns a proxy object that cannot be copied or assigned, but
		// it can be implicitly converted to an XLCellValue object, as shown below.
		// Unfortunately, it is not possible to use the 'auto' keyword, so the XLCellValue
		// type has to be explicitly stated.
		XLCellValue A1 = wks.cell("A1").value();
		XLCellValue B1 = wks.cell("B1").value();
		XLCellValue C1 = wks.cell("C1").value();
		XLCellValue D1 = wks.cell("D1").value();
		XLCellValue E1 = wks.cell("E1").value();

		// The cell value can be implicitly converted to a basic c++ type. However, if the type does not
		// match the type contained in the XLCellValue object (if, for example, floating point value is
		// assigned to a std::string), then an XLValueTypeError exception will be thrown.
		// To check which type is contained, use the .type() method, which will return a XLValueType enum
		// representing the type. As a convenience, the .typeAsString() method returns the type as a string,
		// which can be useful when printing to console.
		/*double vA1 = wks.cell("A1").value();
		int vB1 = wks.cell("B1").value();
		std::string vC1 = wks.cell("C1").value();
		bool vD1 = wks.cell("D1").value();
		double vE1 = wks.cell("E1").value();*/

		// Instead of using implicit (or explicit) conversion, the underlying value can also be retrieved
		// using the .get() method. This is a templated member function, which takes the desired type
		// as a template argument.

		// XLCellValue objects can also be copied and assigned to other cells. This following line
		// will copy and assign the value of cell C1 to cell E1. Note tha only the value is copied;
		// other cell properties of the target cell remain unchanged.
		wks.cell("F1").value() = wks.cell(XLCellReference("C1")).value();
		XLCellValue F1 = wks.cell("F1").value();
		

		// Date/time values is a special case. In Excel, date/time values are essentially just a
		// 64-bit floating point value, that is rendered as a date/time string using special
		// formatting. When retrieving the cell value, it is just a floating point value,
		// and there is no way to identify it as a date/time value.
		// If, however, you know it to be a date time value, or if you want to assign a date/time
		// value to a cell, you can use the XLDateTime class, which falilitates conversion between
		// Excel date/time serial numbers, and the std::tm struct, that is used to store
		// date/time data. See https://en.cppreference.com/w/cpp/chrono/c/tm for more information.

		// An XLDateTime object can be created from a std::tm object:
		std::tm tm;
		tm.tm_year = 121;
		tm.tm_mon = 8;
		tm.tm_mday = 1;
		tm.tm_hour = 12;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		XLDateTime dt(tm);
		//    XLDateTime dt (43791.583333333299);

			// The std::tm object can be assigned to a cell value in the same way as shown previously.
	

		// And as seen previously, an XLCellValue object can be retrieved. However, the object
		// will just contain a floating point value; there is no way to identify it as a date/time value.
		XLCellValue G1 = wks.cell("G1").value();
		
		// If it is known to be a date/time value, the cell value can be converted to an XLDateTime object.
		//auto result = G1.get<XLDateTime>();

	
		// Using the .tm() method, the corresponding std::tm object can be retrieved.
		//auto tmo = result.tm();
	

		doc.save();
		doc.close();
	}


}