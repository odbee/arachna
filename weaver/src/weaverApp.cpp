#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "GraphSetup.h"
#include "GraphHandler.h"
#include "DrawHandler.h"
#include "DataContainer.h"
#include "IniHandler.h"
#include "GuiSetup.h"
using namespace ci;
using namespace ci::app;
using namespace std;

class weaverApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	Graph g;
	DataContainer data;
	GraphHandler GH=GraphHandler(data);
	DrawHandler dh = DrawHandler(GH.g ,data);
	IniHandler ih = IniHandler(data);
	GuiHandler GuiH = GuiHandler(data, ih);

};

void weaverApp::setup()
{
	dh.setupCamera();
	GuiH.setupImGui();
	GH.InitialWebFromObj(0.9, data.fullPath + data.initialGraphDE);

}

void weaverApp::mouseDown( MouseEvent event )
{
}

void weaverApp::update()
{
}

void weaverApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	GuiH.drawParametersWindow();
	{
		dh.drawCamera();
		ci::gl::setMatrices(dh.mCamera);
		dh.drawGraph();



	}
}

CINDER_APP( weaverApp, RendererGl )
