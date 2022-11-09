#include "DataContainer.h"

void DataContainer::setup() {
	fullPath = "";
	initialGraphDE = "initialgraph.txt";
	cyclesDE = "cycles.txt";
	voxelsDE = "voxels.txt";
	anchorPDE = "anchorpoints.txt";
	forbiddenCheck = true;
	runRelax = true;
	carousel = false;
	GLOBALINT = 1;
	highlightCycle = 0;
	nthCycle = 0;
	highlightEdge = false;
}

void DataContainer::updateDirectories() {
}

void DataContainer::loadCycles() {
	std::string filename = fullPath+cyclesDE;

	std::ifstream MyReadFile(filename);
	if (MyReadFile.good()) {
		cycs.clear();
		std::ifstream MyReadFile(filename);
		std::string line;
		while (std::getline(MyReadFile, line)) {
			std::stringstream ss(line);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			std::vector<size_t> cycle;

			if (!vstrings.empty()) {
				for (auto strin = vstrings.begin();
					strin != vstrings.end();
					strin++) {
					//console() << *strin << endl;
					cycle.push_back((size_t)stoi(*strin));
				}
				cycs.push_back(cycle);
			}
		}
	}
	else {
		ci::app::console() << "no cycles file found, check again" << std::endl;
	}
}

void    DataContainer::ExampleAppLog::Clear()
{
	Buf.clear();
	LineOffsets.clear();
	LineOffsets.push_back(0);
}


void    DataContainer::ExampleAppLog::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	int old_size = Buf.size();
	va_list args;
	va_start(args, fmt);
	Buf.appendfv(fmt, args);
	va_end(args);
	for (int new_size = Buf.size(); old_size < new_size; old_size++)
		if (Buf[old_size] == '\n')
			LineOffsets.push_back(old_size + 1);
}


void    DataContainer::ExampleAppLog::Draw(const char* title, bool* p_open)
{

	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();


	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 50), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		Clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf = Buf.begin();
	const char* buf_end = Buf.end();

	{
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

	if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();

}