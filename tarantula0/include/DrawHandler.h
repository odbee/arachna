#pragma once
#include "GraphSetup.h"
#include "DataContainer.h"
#include "cinder/CameraUi.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#include "cinder/CinderImGui.h"
#include "cinder/CameraUi.h"

class DrawHandler
{
private:
	void setup();
public:
	DrawHandler(DataContainer& DATA) : data(DATA) { setup(); }
	DataContainer& data;
	edge_ti ei, eiend;
	vertex_ti vi, viend;
	Graph& g= data.graph;
	ci::mat4 proj;
	ci::vec4 viewp;
	ci::CameraPersp		mCamera;

	ci::gl::BatchRef		mWirePlane;
	ci::gl::BatchRef		mWireCube;
	ci::CameraUi			mCamUi;
	void setupCamera();
	void drawCamera();
	void drawPoints(bool drawNumbers, bool drawVertexPoints, bool drawCycleList);
	void drawGraph( bool colorEdges = false, bool colorTension = true, bool colorLength = false);
	void drawNthCycle();
	void drawNthEdge();
	
};

