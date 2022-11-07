#pragma once
#include "GraphSetup.h"
#include "GraphHandler.h"

class DrawHandler
{
private:
	void setup();
public:
	DrawHandler(GraphHandler& GH) :gh(GH) { setup(); }
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	GraphHandler& gh;
	ci::mat4 proj;
	ci::vec4 viewp;
	Graph &g=gh.g;
	void drawPoints(bool drawNumbers, bool drawVertexPoints, bool drawCycleList);
	void drawGraph( bool colorEdges = false, bool colorTension = true, bool colorLength = false);
	void setupImGui();
	
};

