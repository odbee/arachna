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
#include "ArduPort.h"

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
	ArduPort ap = ArduPort(data,GH);
	GuiHandler GuiH = GuiHandler(data, ih,GH.g,GH.edgesG,ap,GH);
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
			data.selected_edge = data.empty_edge;
		}
		else {
			data.my_log.AddLog("[warning] Max steps received! you can only go back.\n");
			data.INDEX = GH.getRecipeInfo().size();

		}
		
	}
	if (event.getChar() == 'x') { // "c"
		if (data.INDEX > 0) {
			data.selected_edge = data.empty_edge;
			GH.removeEdgeFromRecipe(--data.INDEX);
		}
		else {
			data.my_log.AddLog("[warning] Min steps received! you can only go forward.\n");
			data.INDEX = 0;

		}
		
	}
	if (event.getChar() == 'o') { // "c"
		GH.printOriginalEdges();
		GH.exportOriginalEdges();

	}
	if (event.getChar() == 's') { // "c"
		//serialH.enumerate_ports();

	}
	if (event.getChar() == 'n') { // "c"
		ci::app::console() << "selecting index " << data.selectedOriginalEdgeInd<< std::endl;

		data.selectedOriginalEdgeInd += 1;

	}

	if (event.getChar() == 'p') {
		ap.printEdge(data.selectedOriginalEdgeInd);
		GH.originalEdges[data.selectedOriginalEdgeInd].isDrawn = true;

		//TODO UNSELECTABLE WHEN NOT CONNECTED
	}

	if (event.getChar() == '0') {
		GH.exportCurrentVertices();
		GH.exportCurrentEdges();
	}



}
void weaverApp::setup()
{
	dh.setupCamera();
	GuiH.setupImGui();
	GH.loadEdges(data.fullPath + "edges.csv");
	GH.loadRecipe(data.fullPath + "log.csv");
	GH.setupEdgesGraph();
	GH.initOriginalEdges();
	GH.initAnchorPoints(data.fullPath + data.anchorPDE);
	GH.loadVertices(data.fullPath + "vertices.csv");

	
	GH.InitialWebFromObj(0.9, data.fullPath + data.initialGraphDE);


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
	dh.drawCamera();
	//dh.proj = dh.mCamera.getProjectionMatrix() * dh.mCamera.getViewMatrix();
	//int w = getWindowWidth();
	//int h = getWindowHeight();
	//dh.viewp = vec4(0, h, w, -h); // vertical flip is required
	//dh.drawPoints();

	{
		dh.drawCamera();

		ci::gl::ScopedMatrices push;
		ci::gl::setMatrices(dh.mCamera);
		dh.drawGraph();
		dh.drawNthEdge();
		dh.drawSelectedEdge();
		dh.drawDivisionPoints(GH.edgesG,GH.edgeMap);
		dh.drawAnchorPoints();

	}
}

CINDER_APP( weaverApp, RendererGl )
