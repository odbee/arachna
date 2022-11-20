#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "DataContainer.h"
#include "IniHandler.h"

class GuiHandler
{
private:
	void setup();
public:
	GuiHandler(DataContainer& DATA, IniHandler& IH,Graph&graph) : data(DATA), iniHand(IH),g(graph) { setup(); }
	DataContainer& data;
	Graph& g;
	IniHandler& iniHand;
	void setupImGui();
	void drawParametersWindow();
};
