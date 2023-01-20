#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "DataContainer.h"
#include "IniHandler.h"
#include "ArduPort.h"
#include "GraphHandler.h"
class GuiHandler
{
private:
	void setup();
public:
	GuiHandler(DataContainer& DATA, IniHandler& IH,Graph&graph,EdgesGraph& edgesGraph, ArduPort& arduPort, GraphHandler& GH) : data(DATA), iniHand(IH),g(graph),edgesG(edgesGraph),arduP(arduPort), gh(GH) { setup(); }
	DataContainer& data;
	Graph& g;
	GraphHandler& gh;
	EdgesGraph& edgesG;
	IniHandler& iniHand;
	ArduPort& arduP;
	void setupImGui();
	void drawParametersWindow();
	void drawArduPortTab();
};


