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
	
	int edgeIndex;
	float alpha;
	edge_t highlightedEdge;
	bool highlightEdge;

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
