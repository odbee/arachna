#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/property_map/property_map.hpp>


#include <udgcd.hpp>

#include <GraphSetup.h>

#include <HelpingSetup.h>
#include "AdamsCameraUi.h"
using namespace ci;
using namespace ci::app;
using namespace std;

class HeyCameraUi : virtual public AdamsCameraUi {
	public:
		void printThat() {
			console() << "printed that" << endl;
		}

		virtual void mouseDown(app::MouseEvent& event)
		{
			if (!mEnabled)
				return;
			if (event.isLeft()&&event.isShiftDown()) {
				selected_edge = hovered_edge;
				justclicked = true;

			}
			mouseDown(event.getPos());
			event.setHandled();
		}
		virtual void mouseWheel(app::MouseEvent& event) {
			//int wheelIncrement = event.getWheelIncrement();
			//console() << "comcyc size" << commoncycles.size() << endl;
			//if (!(commoncycles.empty())) {
			//	index_in_commoncyc = (index_in_commoncyc+ wheelIncrement)%commoncycles.size();
			//	console() << "index in cycles" << index_in_commoncyc<< endl;
			//}
			
			
		}

		virtual void mouseDown(const vec2& mousePos) 
		{
			mLastAction = ACTION_NONE;
			if (!mCamera || !mEnabled)
				return;
			mInitialMousePos = mousePos;
			mInitialCam = *mCamera;
			mInitialPivotDistance = mCamera->getPivotDistance();
			
		}
};

class cyclistApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove(MouseEvent event) override;
	void mouseWheel(MouseEvent event) override;
	void keyDown(KeyEvent event) override;

	void update() override;
	void draw() override;
	CameraPersp		mCamera;

private:
	gl::BatchRef		mWirePlane;
	gl::BatchRef		mWireCube;
	HeyCameraUi			mCamUi;
	vec3 camPos = { 30.0f, 10.0f, 40.0f };
	string FILEPATH="D:\\Dokumente\\Uni\\programmieren\\cindertesting\\tarantula2\\vc2019\\startmesh.obj";
	vec2 mMousePos;
	string CYCLEORDER = "";
	string CYCLEFILENAME = "";
	string CYCLEIMPORTFILENAME = "";
};



void cyclistApp::setup()
{
	mCamera.lookAt(vec3(30.0f, 20.0f, 40.0f), vec3(0));
	mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
	
	mCamUi.setCamera(&mCamera);
	mCamUi.setWindowSize( { 640,480 });
	mCamUi.setMouseWheelMultiplier(1.2f);
	mCamUi.setMinimumPivotDistance(1.0f);
	mCamUi.enable();
	mCamUi.connect(getWindow());
	mCamUi.printThat();
	auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
	auto colorShader = gl::getStockShader(gl::ShaderDef().color());
	mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(11)).subdivisions(ivec2(10)), colorShader);
	mWireCube = gl::Batch::create(geom::WireCube(), colorShader);
	ImGui::Initialize();
	ImGuiIO& io = ImGui::GetIO();
	gl::lineWidth(2);

}
void cyclistApp::mouseMove(MouseEvent event)
{
	// Keep track of the mouse.
	//console() << "ouse moved" << endl;
	mMousePos = event.getPos();
}

void cyclistApp::mouseWheel(MouseEvent event) {
	int wheelIncrement = event.getWheelIncrement();
	//console() << "wheel inc" << wheelIncrement << endl;
	if (!(commoncycles.empty())) {
		console() << "commoncycles size " << commoncycles.size() << endl;
		index_in_commoncyc = (index_in_commoncyc + wheelIncrement) % commoncycles.size();
		console() << "index in cycles" << index_in_commoncyc << endl;
	}
}
void cyclistApp::mouseDown( MouseEvent event )
{
	console() << "changed edge " << endl;


	
	
}

void cyclistApp::keyDown(KeyEvent event) {
	if (selected_edge != empty_edge) {
		if (event.getCode() == KeyEvent::KEY_UP) {
			index_in_commoncyc = (index_in_commoncyc + 1) % commoncycles.size();
		}
		if (event.getCode() == KeyEvent::KEY_DOWN) {
			index_in_commoncyc = (index_in_commoncyc - 1) % commoncycles.size();
		}
	}
	
	


}

void cyclistApp::update()
{
	if (selected_edge != empty_edge&&justclicked) {

		commoncycles = compareVectorsReturnIntersection(cyclesPm[boost::source(selected_edge, g)], cyclesPm[boost::target(selected_edge, g)]);
		index_in_commoncyc = 0;
		cycleN = commoncycles[index_in_commoncyc];
	}
	
	if (!commoncycles.empty()) {
		cycleN = commoncycles[index_in_commoncyc];
	}
	justclicked = false;
}


void cyclistApp::draw()
{
	gl::clear(Color::hex(0x282828));

	bool isopen;

	ImGui::Begin("Params", &isopen/*, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize*/);
	ImGui::Text("hello9");
	ImGui::InputText("import file path ", &FILEPATH);
	ImGui::InputInt("cycle number", &cycleN);
	if (!cycles.empty()) {
		if (cycleN >= cycles.size() || cycleN < 0) {
			cycleN = cycleN % cycles.size();
		}
	}
	
	if (ImGui::Button("load pointcloud")) {
		g.clear();
		cycles.clear();

		InitialWebFromObj(&g, 0.8, FILEPATH, cycles);
	}

	if (ImGui::Button("compute cycles")) {
		
		cycles = udgcd::findCycles<Graph, vertex_t>(g);
		addCyclesToVertices(&g, cycles);
	}

	ImGui::InputText("cycle import file name ", &CYCLEIMPORTFILENAME);
	if (ImGui::Button("load cycles")) {

		loadcycles(&g, cycles, CYCLEIMPORTFILENAME);

	}


	if (ImGui::Button("remove active cycle")) {

		removeActiveCycle(&g, cycles, cycleN,index_in_commoncyc, commoncycles);
	}
	ImGui::InputText("new cycle input ", &CYCLEORDER);
	if (ImGui::Button("add cycle")) {

		addCycle(&g, cycles, CYCLEORDER, cycleN, index_in_commoncyc, commoncycles);
		CYCLEORDER.clear();
	}
	ImGui::InputText("cycle export file name ", &CYCLEFILENAME);
	if (ImGui::Button("save cycles")) {

		savecycles(cycles,CYCLEFILENAME);
		
	}
	//ImGui::StyleColorsLight();

	ImGui::End();

	//gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
		int w = getWindowWidth();
		int h = getWindowHeight();
		vec4 viewport = vec4(0, h, w, -h); // vertical flip is required
		if (!cycles.empty()) {
			drawPoints(&g, projection, viewport);
		}
		

		gl::ScopedMatrices push;
		gl::setMatrices(mCamera);

		{
			gl::color(1.0f, 1.0f, 1.0f, 0.1f);

			drawGraph(&g, projection, viewport,ColorA(255.0f / 255, 214.0f / 255, 235.0f / 255, 1.0f));
			if (!cycles.empty()){
				drawCycleEdges(&g, projection, viewport, cycles, cycleN,Color::hex(0xCC241D));
			}
			float u = mMousePos.x / (float)getWindowWidth();
			float v = mMousePos.y / (float)getWindowHeight();
			Ray ray = mCamera.generateRay(u, 1.0f - v, mCamera.getAspectRatio());

			
			//for (size_t i = 0; i < 100; i++)
			//{
			//	gl::drawCube(ray.getOrigin() + i * 10.0f * ray.getDirection(), { 4.0f,4.0f,4.0f });
			//}
			if (!cycles.empty()){
				hovered_edge = getClosestEdgeFromRay(ray, &g);
				
				gl::color(ColorA(0.0f,1.0f,0.0f,0.4f));
				gl::drawLine(position[boost::source(hovered_edge, g)], position[boost::target(hovered_edge, g)]);
			}





		}

	}
	//			END CAMERA DRAW
}

CINDER_APP( cyclistApp, RendererGl )
