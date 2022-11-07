#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"


class GuiHandler
{
	private:
		void setup();
	public:
		GuiHandler() { setup(); }
		std::string initialGraphDE, cyclesDE, voxelsDE, anchorPDE, fullPath;
		bool checkForbidden, runRelax, carousel, runCycles;
		int edgeIndex;
		float alpha;
		float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
		float G_cachedensity = 0.0005f, G_cachelength = .999f, G_cachetension = 0.0005f;
		std::vector<float*> vallist{ &G_density,&G_length,&G_tension };
		std::vector<float*> cachelist{ &G_cachedensity,&G_cachelength,&G_cachetension };
		
		std::string makePath(std::string directoryExtension);
		void adjustothers(std::vector<float*>& values, std::vector<float*>& cachedvalues, size_t index);
		void checkforchange(std::vector<float*>& values, std::vector<float*>& cachedvalues);
		void setupImGui();
		void drawParametersWindow();
};

