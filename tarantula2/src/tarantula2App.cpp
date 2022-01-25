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

#include "Resetter.h"

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
	bool drawNCycle = false;


	std::pair<edge_t, bool> c, d;

	string profilername="profiler.csv";
	ofstream profile_out;
	int cachediter;
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
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../resources/fonts/RedHatText-Medium.ttf", 18);

	ImGui::GetStyle().WindowRounding = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().WindowPadding.x = 30.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().WindowPadding.y = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().FramePadding.y = 5.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().FramePadding.x = 8.0f;// <- Set this on init or use ImGui::PushStyleVar()

	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 15.0f;
	ImGui::GetStyle().GrabRounding = 20.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 12.0f;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::GetStyle().FrameBorderSize = 0.0f;
	ImGui::GetStyle().ChildBorderSize = 0.0f;
	ImGui::GetStyle().GrabMinSize = 20.0f;


	initVoxelMap("voxelvals.txt");
	console() << "voxel map initiated successfully, value of {0,0,0} is:" << voxelMap[{0, 0, 0}] << endl;
	InitialWebFromPc(&g,relaxc, "txtf.txt");
	std::ofstream ofs;

	profile_out.open(profilername, std::ios_base::trunc);
	profile_out << "counter,command,duration" << endl;
	profile_out.close();

	addcyclesfromPc(relaxc, g, cycles);
}

void tarantula2App::keyDown(KeyEvent event)
{
	if (event.getCode() == 99) { // "c"
		//addRandomEdge(&g, relaxc);
		auto start = std::chrono::high_resolution_clock::now();
		addRandomCyclicEdgeTesting(&g, relaxc, &cycles);
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end- start);
		profile_out.open(profilername, std::ios_base::app);
		profile_out << iterationcounter << "," << "addcyc" << "," << duration.count() << endl;
		profile_out.close();
		iterationcounter++;
	}
	if (event.getChar() == 'm') { // "c"
	//addRandomEdge(&g, relaxc);
		for (int i = 0; i < 50; i++)
		{
			addRandomCyclicEdgeTesting(&g, relaxc, &cycles);

			iterationcounter++;
		}
	}


	if (event.getCode() == 107) {
		console() << stringfromCycles(cycles) << endl;
	}

	if (event.getChar() == 'p') {
		exportGraph(g);
	}
	if (event.getChar() == 'x') {
		getVertsFromFile("textfile.txt");
	}

	//console() << event.getCode() << endl;
}

void tarantula2App::update()
{
	auto start = std::chrono::high_resolution_clock::now();
	relaxPhysics(&g);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	
	if (cachediter != iterationcounter) {
		profile_out.open(profilername, std::ios_base::app);
		profile_out << iterationcounter << "," << "physics" << "," << duration.count() << endl;
		profile_out.close();

	}
	
	cachediter = iterationcounter;
}

using namespace ImGui;        static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };


void tarantula2App::draw()
{
	bool isopen;
	ImGui::Begin("Params",&isopen, 3);
	ImGui::StyleColorsDark();
	ImGui::SetNextItemOpen(true);

	if (ImGui::CollapsingHeader("Settings")) {
		ImGui::Text("number of iterations = %i", iterationcounter);

		ImGui::SliderFloat("density", &G_density, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("length", &G_length, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("tension", &G_tension, 0.0001f, .999f, "%.2f");


		checkforchange(vallist, cachelist);
		ImGui::Checkbox("check for forbidden edges", &CHECKFORBIDDEN);
		if (ImGui::Button("Reset web")) {
			console() << "resetting web" << endl;
			resetweb(g, relaxc, cycles);
		}
			
	}
	
	ImGui::Dummy(ImVec2(0.0f, 20.0f));



	if (ImGui::CollapsingHeader(" Debug Settings")) {

		ImGui::InputInt("cycle number", &displayCycle_i);
		//if (displayCycle_i >= cycles.size()) {
		//	displayCycle_i = displayCycle_i % cycles.size();
		//}
		ImGui::Checkbox("Draw Vertices", &mDrawVertices);


		ImGui::Checkbox("Draw Edge Numbers", &mDrawNumbers);
		ImGui::Checkbox("Draw Vertex Info", &mDrawVertexInfo);
		ImGui::Checkbox("Color Edges", &colorEdges);
		ImGui::Checkbox("highlight Nth Cycle", &drawNCycle);
		ImGui::Checkbox("Color Edge Tensions", &colorTens);
	}
	ImGui::End();



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
				//drawCycleEdges(&g, projection, viewport, cycles, displayCycle_ii, Color(1.0f,0.6f,0.0f));
			}


			
		}

	}
	//			END CAMERA DRAW
}


CINDER_APP( tarantula2App, RendererGl )
