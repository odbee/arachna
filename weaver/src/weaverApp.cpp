#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "GraphSetup.h"
#include "GraphHandler.h"
#include "DrawHandler.h"
#include "DataContainer.h"
#include "IniHandler.h"
#include "GuiSetup.h"
#include "cinder/CameraUi.h"
using namespace ci;
using namespace ci::app;
using namespace std;



class weaverApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
	DataContainer data;
	GraphHandler GH=GraphHandler(data);
	DrawHandler dh = DrawHandler(GH.g ,data);
	IniHandler ih = IniHandler(data);
	GuiHandler GuiH = GuiHandler(data, ih,GH.g);
	ci::vec2 mMousePos;
};

void weaverApp::mouseMove(MouseEvent event) {
	mMousePos = event.getPos();
	//ci::app::console() << "mouse moved" << std::endl;
}
void weaverApp::keyDown(KeyEvent event) {
	if (event.getCode() == 99) { // "c"
		if (data.INDEX < GH.getRecipeInfo().size()) {
			GH.addEdgeFromRecipe(data.INDEX++);
		}
		else {
			data.my_log.AddLog("[warning] Max steps received! you can only go back.\n");
			data.INDEX = GH.getRecipeInfo().size();

		}
		
	}
	if (event.getChar() == 'x') { // "c"
		if (data.INDEX > 0) {
			GH.removeEdgeFromRecipe(--data.INDEX);
		}
		else {
			data.my_log.AddLog("[warning] Min steps received! you can only go forward.\n");
			data.INDEX = 0;

		}
		
	}
}
void weaverApp::setup()
{
	dh.setupCamera();
	GuiH.setupImGui();
	GH.loadEdges(data.fullPath + "edges.csv");
	GH.loadRecipe(data.fullPath + "log.csv");
	GH.InitialWebFromObj(0.9, data.fullPath + data.initialGraphDE);


	GH.loadVertices(data.fullPath + "vertices.csv");

	GH.AddAllVerts();

}

void weaverApp::mouseDown( MouseEvent event )
{
	ci::app::console() << "clicking old item" << std::endl;

}

void weaverApp::update()
{
	
	GH.relaxPhysics();
	data.hovered_edge = GH.getClosestEdgeFromRay(dh.calculateRay(mMousePos));

}

void weaverApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	GuiH.drawParametersWindow();
	{
		dh.drawCamera();
		ci::gl::setMatrices(dh.mCamera);
		dh.drawGraph();
		dh.drawNthEdge();
		dh.drawSelectedEdge();

		//gl::color(ColorA(0.0f, 1.0f, 0.0f, 0.4f));

	}
}

CINDER_APP( weaverApp, RendererGl )
