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
		voxel_map voxels;
		anchor_points anchorPoints;
		Graph graph;
		cycleList cycs;
		std::string initialGraphDE, cyclesDE, voxelsDE, anchorPDE, fullPath;
		bool forbiddenCheck, runRelax, carousel, runCycles;
		bool highlightCycle;
		int nthCycle;
		int GLOBALINT;
		int edgeIndex;
		float alpha;
		edge_t highlightedEdge;
		bool highlightEdge;
		float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
		float G_cachedensity = 0.0005f, G_cachelength = .999f, G_cachetension = 0.0005f;
		std::vector<float*> vallist{ &G_density,&G_length,&G_tension };
		std::vector<float*> cachelist{ &G_cachedensity,&G_cachelength,&G_cachetension };

		void updateDirectories();
		void loadCycles();

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

