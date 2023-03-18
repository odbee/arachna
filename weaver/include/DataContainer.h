#pragma once
#include <vector>
#include <string>
#include <map>
#include <array>

#include "cinder/app/App.h"
#include "GraphSetup.h"
#include "cinder/CinderImGui.h"


class DataContainer
{
private:
	void setup();
public:
	DataContainer() { setup(); }
	
	std::string initialGraphDE, cyclesDE, voxelsDE, anchorPDE, fullPath;
	int INDEX = 0;

	signed int  edgeIndex;
	float alpha;
	edge_t 
		edEdge;
	bool highlightEdge;
	edge_t hovered_edge;
	std::string customEdges;
	edge_t selected_edge;
	edge_t empty_edge;
	bool justclicked = false;
	bool is_hovered = false;
	int selectedOriginalEdgeInd = 0;
	std::string COMPort;
	struct ExampleAppLog
	{
		ImGuiTextBuffer     Buf;

		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.

		ExampleAppLog()
		{
			AutoScroll = true;
			Clear();
		}

		void    Clear();

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2);

		void    Draw(const char* title, bool* p_open = NULL);
	};

	ExampleAppLog my_log;
};
