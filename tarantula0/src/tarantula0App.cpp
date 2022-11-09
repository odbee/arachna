#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/property_map/property_map.hpp>


#include "GraphSetup.h"
#include "GraphHandler.h"
#include "GuiSetup.h"
#include "DrawHandler.h"
#include "VoxelHandler.h"
#include "AnchorPointsHandler.h"
#include "DataContainer.h"

// TODO split code into UI, class and values, and Graph class and values.



using namespace ci;
using namespace ci::app;
using namespace std;

class tarantula0App : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	

	
	Graph g;
	



	DataContainer data;
	GraphHandler GH = GraphHandler(data);
	VoxelHandler VHHH=VoxelHandler(data);
	AnchorPointsHandler AH=AnchorPointsHandler(data);
	DrawHandler dh = DrawHandler(data);
	IniHandler ih = IniHandler(data);
	GuiHandler GuiH=GuiHandler(data,ih);

};

void tarantula0App::setup()
{
	//			CAMERA SETUP
	dh.setupCamera();
	//			END CAMERA SETUP

	GuiH.setupImGui();
	
	//data.my_log.AddLog("current directory is: ");
	//data.my_log.AddLog(data.fullPath.c_str());
	//data.my_log.AddLog("\n");
	GH.InitialWebFromObj(0.9, data.fullPath + data.initialGraphDE);
	VHHH.initVoxelMap(data.fullPath+data.voxelsDE);
	data.loadCycles();
	GH.addCyclesToVertices();

	
}

void tarantula0App::mouseDown( MouseEvent event )
{

}

void tarantula0App::keyDown(KeyEvent event)
{
	if (event.getCode() == 99) { // "c"
//addRandomEdge(&g, relaxc);
		GH.addRandomCyclicEdgeTesting(.9);


	}

	if (event.getChar() == 'k') {
		edge_ti ei, eiend;
		std::tie(ei, eiend) = boost::edges(data.graph);
		data.highlightedEdge=GH.pickrandomEdge(ei, eiend);
		data.highlightEdge = true;
	}
}


void tarantula0App::update()
{
	GH.relaxPhysics();
}

void tarantula0App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	GuiH.drawParametersWindow();


	//			CAMERA DRAW
	{
		dh.drawCamera();
		ci::gl::setMatrices(dh.mCamera);

		dh.drawGraph();
		dh.drawNthCycle();
		dh.drawNthEdge();


	}
	//			END CAMERA DRAW
}

CINDER_APP( tarantula0App, RendererGl )
