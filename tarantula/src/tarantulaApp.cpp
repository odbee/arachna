#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "GraphSetup.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/property_map/property_map.hpp>
#include <udgcd.hpp>
#include <typeinfo>

using namespace ci;
using namespace ci::app;
using namespace std;

class tarantulaApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	//void mouseDown(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	CameraPersp		mCamera;
private:
	gl::BatchRef		mWirePlane;
	CameraUi			mCamUi;
	float relaxc = 0.8f;
	bool hasCycle=false;
	std::vector<std::vector<size_t>> cycles;

};


void tarantulaApp::setup()
{
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


	setInitialWeb(&g, relaxc);
}

void tarantulaApp::keyDown(KeyEvent event) {
	console() << ":" << event.getCode() << ":" << endl;
	if (event.getCode() == 99) {
		//addRandomEdge(&g, relaxc);
		addRandomCyclicEdge(&g, relaxc, &cycles);
		if (hasCycle==false)
		{
			cycles = udgcd::findCycles<Graph, vertex_t>(g);
			cycles[0];
			hasCycle = cycles.size();
			if (hasCycle) {
				addCyclesToVertices(&g, cycles);
			}
		}
		udgcd::printPaths(console(),cycles);
		
	}
	if (event.getCode() == 107) {
		for (int i = 0; i < 100; i++)
		{
			addRandomEdge(&g, relaxc);
		}
		
	}

	if (event.getChar() == 'p') {
		console() << hasCycle << endl;
	}

}
void tarantulaApp::update()
{
	relaxPhysics(&g);
}

void tarantulaApp::draw()
{

	gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
		int w = getWindowWidth();
		int h = getWindowHeight();
		vec4 viewport = vec4(0, h, w, -h); // vertical flip is required
		drawPoints(&g, projection, viewport);
		gl::ScopedMatrices push;
		gl::setMatrices(mCamera);
		{	
			//mWirePlane->draw();
			drawGraph(&g, projection, viewport);
		}
	}
	//			END CAMERA DRAW
}
CINDER_APP(tarantulaApp, RendererGl)