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

#include <udgcd.hpp>


#include "GraphSetup.h"
#include "HelperFunctions.h"
#include "GraphFunctions.h"
#include "PhysicsFunctions.h"
#include "DrawFunctions.h"
#include "TextHelper.h"
#include "InitialWebs.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class tarantula2App : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;
	CameraPersp		mCamera;
private:
	gl::BatchRef		mWirePlane;
	CameraUi			mCamUi;
	float relaxc = 0.2f;
	bool hasCycle = false;	
	bool mDrawVertices=false;
	bool mDrawNumbers = false;
	bool mDrawVertexInfo = false;
	bool colorEdges = false;
	bool colorTens = false;
	bool drawNCycle = true;




};

void tarantula2App::setup()
{
	console() << endl;
	//			CAMERA SETUP
	{
		mCamera.lookAt(vec3(30.0f, 20.0f, 40.0f), vec3(0));
		mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
		mCamUi = CameraUi(&mCamera, getWindow());
		auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
		auto colorShader = gl::getStockShader(gl::ShaderDef().color());
		mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(10)).subdivisions(ivec2(10)), colorShader);
	}
	//			END CAMERA SETUP


	ImGui::Initialize();


	setInitialWeb(&g, relaxc);
}

void tarantula2App::keyDown( KeyEvent event )
{
	if (event.getCode() == 99) { // "c"
		//addRandomEdge(&g, relaxc);
		addRandomCyclicEdge(&g, relaxc, &cycles);
		iterationcounter++;
		//console() << stringfromCycles(cycles) << endl;


		//if (hasCycle == false) // find cycles every time it has no cycles
		//{
		//	cycles = udgcd::findCycles<Graph, vertex_t>(g);
		//	hasCycle = cycles.size();
		//	if (hasCycle) {
		//		addCyclesToVertices(&g, cycles);
		//		udgcd::printPaths(console(), cycles);
		//	}

		//}
		//udgcd::printPaths(console(),cycles);
	}


	if (event.getCode() == 107) {
		console() << stringfromCycles(cycles) << endl;
	}

	if (event.getChar() == 'p') {
		exportGraph(g);
	}
}

void tarantula2App::update()
{
	relaxPhysics(&g);
}



void tarantula2App::draw()
{
	ImGui::Text("number of iterations = %i", iterationcounter);
	ImGui::InputInt("cycle number", &displayCycle_i);
	if (displayCycle_i >= cycles.size()) {
		displayCycle_i = displayCycle_i % cycles.size();
	}
	ImGui::Checkbox("Draw Vertices", &mDrawVertices);
	
	
	ImGui::Checkbox("Draw Edge Numbers", &mDrawNumbers);
	ImGui::Checkbox("Draw Vertex Info", &mDrawVertexInfo);
	ImGui::Checkbox("Color Edges", &colorEdges);
	ImGui::Checkbox("highlight Nth Cycle", &drawNCycle);
	ImGui::Checkbox("Color Edge Tensions", &colorTens);

	ImGui::Checkbox("check for forbidden edges", &CHECKFORBIDDEN);



	gl::clear( Color( 0, 0, 0 ) ); 
	//gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
		int w = getWindowWidth();
		int h = getWindowHeight();
		vec4 viewport = vec4(0, h, w, -h); // vertical flip is required

		drawPoints(&g, projection, viewport, mDrawNumbers, mDrawVertices, mDrawVertexInfo);
		if (drawNCycle)
			drawCycle(&g, projection, viewport, cycles, displayCycle_i);
		gl::ScopedMatrices push;
		gl::setMatrices(mCamera);
		{
			//mWirePlane->draw();
			drawGraph(&g, projection, viewport, colorEdges,colorTens);
			if (drawNCycle) {
				drawCycleEdges(&g, projection, viewport, cycles, displayCycle_i);
				drawCycleEdges(&g, projection, viewport, cycles, displayCycle_ii, Color(1.0f,0.6f,0.0f));
			}
			gl::color(0.0f, 0.0f, 1.0f,0.6f);
			gl::drawLine(position[displayEdgeV_ii], position[displayEdgeV_i]);
			gl::drawLine(position[displayEdgeV_iii], position[displayEdgeV_iv]);
			//gl::drawLine(position[displayEdgeV_i], position[displayEdgeV_iii]);
			//gl::drawLine(position[displayEdgeV_iv], position[displayEdgeV_ii]);


			
		}
	}
	//			END CAMERA DRAW
}


CINDER_APP( tarantula2App, RendererGl )
