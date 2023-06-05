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
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class tarantulaApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void render();
	//void mouseDown(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	CameraPersp		mCamera;
private:
	gl::BatchRef		mWirePlane;
	CameraUi			mCamUi;
	float relaxc = 0.8f;
	bool hasCycle=false;
	std::vector<std::vector<size_t>> cycles;
	string				mText;
	gl::TextureRef		mTextTexture;
	vec2				mSize;
	Font				mFont;
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

	mText = "h";

#if defined( CINDER_COCOA )
	mFont = Font("Cochin-Italic", 32);
#else
	mFont = Font("Times New Roman", 24);
#endif
	mSize = vec2(300, 100);
	render();

	setInitialWeb(&g, relaxc);
}

void tarantulaApp::render()
{
	TextBox tbox = TextBox().alignment(TextBox::LEFT).font(mFont).size(ivec2(mSize.x, TextBox::GROW)).text(mText);
	tbox.setColor(Color(1.0f, 0.65f, 0.35f));
	tbox.setBackgroundColor(ColorA(0.5, 0, 0, 1));
	ivec2 sz = tbox.measure();
	mTextTexture = gl::Texture2d::create(tbox.render());
}



void tarantulaApp::keyDown(KeyEvent event) {
	console() << ":" << event.getCode() << ":" << endl;
	if (event.getCode() == 99) { // "c"
		addRandomEdge(&g, relaxc);
		//addRandomCyclicEdge(&g, relaxc, &cycles,mText);
		
		if (hasCycle==false) // find cycles every time it has no cycles
		{
			cycles = udgcd::findCycles<Graph, vertex_t>(g);
			cycles[0];
			hasCycle = cycles.size();
			if (hasCycle) {
				addCyclesToVertices(&g, cycles);
				udgcd::printPaths(console(), cycles);
			}

		}
		//udgcd::printPaths(console(),cycles);
		
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
	render();
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
	if (mTextTexture)
		gl::draw(mTextTexture);
	//			END CAMERA DRAW
}
CINDER_APP(tarantulaApp, RendererGl)