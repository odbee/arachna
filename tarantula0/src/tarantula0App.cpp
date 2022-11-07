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
	VoxelHandler VHHH;
	AnchorPointsHandler AH;
	GraphHandler GH = GraphHandler(VHHH,AH);
	DrawHandler dh = DrawHandler(GH);
	GuiHandler GuiH;
};

void tarantula0App::setup()
{
	GuiH.setupImGui();

	
}

void tarantula0App::mouseDown( MouseEvent event )
{
}

void tarantula0App::keyDown(KeyEvent event)
{
}


void tarantula0App::update()
{
}

void tarantula0App::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	GuiH.drawParametersWindow();
}

CINDER_APP( tarantula0App, RendererGl )
