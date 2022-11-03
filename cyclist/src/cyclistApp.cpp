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

#include <CachedData.h>
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

	void mouseMove(MouseEvent event) override;
	void keyDown(KeyEvent event) override;

	void update() override;
	void draw() override;
	CameraPersp		mCamera;


private:
	gl::BatchRef		mWirePlane;
	gl::BatchRef		mWireCube;
	HeyCameraUi			mCamUi;
	vec3 camPos = { 30.0f, 10.0f, 40.0f };
	string FILEPATH="startmesh.obj";
	
	IniHandler iniHand;
	vec2 mMousePos;
	string CYCLEORDER = "";
	string CYCLEFILENAME = "";
	string CYCLEIMPORTFILENAME = "";
	bool redText = true;
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
	iniHand.initImGui("cached_ui.ini");


	
}
void cyclistApp::mouseMove(MouseEvent event)
{
	// Keep track of the mouse.
	//console() << "ouse moved" << endl;
	mMousePos = event.getPos();
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
		if (!commoncycles.empty()) {
			cycleN = commoncycles[index_in_commoncyc];
		}
	}
	
	if (!commoncycles.empty()&&selected_edge!=empty_edge) {
		cycleN = commoncycles[index_in_commoncyc];
	}
	justclicked = false;
}


void cyclistApp::draw()
{
	//gl::clear(Color::hex(0x282828));
	gl::clear(Color::black());

	bool isopen;
	// IMGUI
	{
		ImGui::Begin("Params", &isopen/*, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize*/);
		my_log.Draw("title");
		ImGui::PushItemWidth(200);




		if (ImGui::InputText("import directory path ", &dirPath)) {
			iniHand.overwriteTagImGui("graph directory", dirPath);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::Separator();
		ImGui::SameLine();
		if (ImGui::Button("load pointcloud")) {
			//g.clear();
			//cycles.clear();
			InitialWebFromPc(&g, 0.9, dirPath +PCDIRECTORYEXTENSION);
			
			addcyclesfromPc(0.9, g, cycles);
			//console() << stringfromCyclesShort(cycles) << endl;
		}

		ImGui::Separator();
		ImGui::SameLine();

		if (ImGui::Button("load starting graph")) {
			//g.clear();
			//cycles.clear();

			InitialWebFromFile(&g, 0.8, dirPath + INITIALGRAPHDIRECTORYEXTENSION, cycles);

		}
		ImGui::SameLine();

		if (ImGui::Button("compute cycles")) {
			if (g.m_vertices.empty()) {
				my_log.AddLog("[error] No graph found to compute cycles. try adding a graph first.\n");
			}
			else {

				if (!cycles.empty())
				{
					my_log.AddLog("[error] cycles already exist! removing cycles before proceeding.\n");
					boost::graph_traits< Graph >::vertex_iterator vi, viend;
					cycles.clear();
					for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
						cyclesPm[*vi].clear();
					}
				}
				cycles = udgcd::findCycles<Graph, vertex_t>(g);
				addCyclesToVertices(&g, cycles);
				my_log.AddLog("[info]successfully computed cycles.\n");
			}


		}
		ImGui::SameLine();

		if (ImGui::Button("load cycles")) {
			if (!cycles.empty())
			{
				my_log.AddLog("[error] cycles already exist! removing cycles before proceeding.\n");
				boost::graph_traits< Graph >::vertex_iterator vi, viend;
				cycles.clear();
				for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
					cyclesPm[*vi].clear();
				}
			}
			loadcycles(&g, cycles, dirPath + CYCLESDIRECTORYEXTENSION);
			my_log.AddLog("[info]successfully loaded cycles.\n");
		}

		ImGui::Separator();

		ImGui::PushItemWidth(100);
		if (ImGui::InputInt("cycle number", &cycleN)) {
			selected_edge = empty_edge;
		}
		
		ImGui::PopItemWidth();

		if (!cycles.empty()) {
			if (cycleN >= cycles.size() || cycleN < 0) {
				cycleN = cycleN % cycles.size();
			}
		}

		


		if (ImGui::Button("remove active cycle")) {
			if (!cycles.empty()) {
				removeActiveCycle(&g, cycles, cycleN, index_in_commoncyc, commoncycles);
			}
			else {
				my_log.AddLog("[error] cannot remove cycle! no cycles available.\n");
			}

		}
		ImGui::InputText("new cycle input ", &CYCLEORDER);
		if (ImGui::Button("add cycle")) {
			
			addCycle(&g, cycles, CYCLEORDER, cycleN, index_in_commoncyc, commoncycles);
			CYCLEORDER.clear();
		}
		if (ImGui::Button("save cycles")) {

			savecycles(cycles, dirPath + CYCLESDIRECTORYEXTENSION);
			my_log.AddLog("[info]successfully saved cycles.\n");


		}
		if (ImGui::Button("save graph")) {

			exportGraphTXT(g,dirPath+INITIALGRAPHDIRECTORYEXTENSION);
			my_log.AddLog("[info]successfully saved  graph.\n");


		}

		
		ImGui::Checkbox("draw red text", &redText);
		//ImGui::StyleColorsLight();

		ImGui::End();
	}
	//gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
		int w = getWindowWidth();
		int h = getWindowHeight();
		vec4 viewport = vec4(0, h, w, -h); // vertical flip is required
		if (!cycles.empty()) {
			drawPoints(&g, projection, viewport, redText);
		}
		

		gl::ScopedMatrices push;
		gl::setMatrices(mCamera);

		{
			gl::color(1.0f, 1.0f, 1.0f, 0.1f);

			drawGraph(&g, projection, viewport,ColorA(255.0f / 255, 214.0f / 255, 235.0f / 255, 1.0f));
			if (!cycles.empty()){
				if (cycles[cycleN].size()) {
					drawCycleEdges(&g, projection, viewport, cycles, cycleN, Color::hex(0xCC241D));
				}
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
				
				if (hovered_edge != selected_edge) {
					gl::color(ColorA(0.0f, 1.0f, 0.0f, 0.4f));

					gl::drawLine(position[boost::source(hovered_edge, g)], position[boost::target(hovered_edge, g)]);

				} 
				else {
					
				}
				gl::color(Color::hex(0x458588));

				gl::drawLine(position[boost::source(selected_edge, g)], position[boost::target(selected_edge, g)]);

			}




		}

	}
	//			END CAMERA DRAW
}

CINDER_APP( cyclistApp, RendererGl )
