#pragma once
#include "GraphSetup.h"

#include "cinder/CameraUi.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#include "cinder/CinderImGui.h"
#include "cinder/CameraUi.h"
#include "DataContainer.h"

#include "AdamsCameraUi.h"

#include <numeric>



class DrawHandler
{
private:
	void setup();
public:
	DrawHandler(Graph& graph, DataContainer& DATA) : g(graph), data(DATA) { setup(); }
	DataContainer& data;
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	Graph& g;
	ci::mat4 proj;
	ci::vec4 viewp;
	ci::CameraPersp		mCamera;

	ci::gl::BatchRef		mWirePlane;
	ci::gl::BatchRef		mWireCube;
	ci::AdamsCameraUi			mCamUi = ci::AdamsCameraUi(data);
	void mouseDown(ci::app::MouseEvent& event) ;
	void setupCamera();
	ci::Ray calculateRay(ci::vec2 mousePos);
	void drawCamera();
	void drawGraph(bool colorEdges = false, bool colorTension = true, bool colorLength = false);
	void drawNthEdge();
	void drawSelectedEdge();
	void drawDivisionPoints(EdgesGraph& edgesGraph ,std::map<int, EdgeContainer>& edgeMap);
	void drawPoints();
};
