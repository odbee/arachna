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
#include "quickhull/QuickHull.hpp"


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

	float density=0.0005f, length=.999f, tension=0.0005f;
	vector<float*> vallist{ &density,&length,&tension };

	float cachedensity = 0.0005f, cachelength = .999f, cachetension = 0.0005f;
	vector<float*> cachelist{ &cachedensity,&cachelength,&cachetension };
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
	ImGui::GetStyle().WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 0.0f;
	ImGui::GetStyle().GrabRounding = 0.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 0.0f;

	initVoxelMap("voxelvals.txt");
	console() << "voxel map initiated successfully, value of {0,0,0} is:" << voxelMap[{0, 0, 0}] << endl;
	InitialWebFromPc(&g,relaxc, "txtf.txt");
	std::ofstream ofs;

	profile_out.open(profilername, std::ios_base::trunc);
	profile_out << "counter,command,duration" << endl;
	profile_out.close();


	{
		using FloatType = float;
		using vex3 = quickhull::Vector3<FloatType>;

		quickhull::QuickHull<FloatType> qh;
		std::vector<vex3> pc;

		for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

			pc.emplace_back(float(position[*vi][0]), float(position[*vi][1]), float(position[*vi][2]));

		}
		auto hull = qh.getConvexHull(&pc[0].x, pc.size(), true, true);
		auto indexbuffer = hull.getIndexBuffer();
		for (size_t i = 0; i < indexbuffer.size() / 3; i++)
		{
			convhull.push_back({ indexbuffer.begin() + 3 * i,indexbuffer.begin() + 3 * i + 3 });
		}

		for (const auto& cycl : convhull) {
			for (size_t i = 0; i < cycl.size(); i++)
			{
				auto v1 = cycl[i];
				auto v2 = cycl[(i + 1) % cycl.size()];
				//console() << v1 << "," << v2 << endl;
				typename boost::graph_traits<Graph>::adjacency_iterator ai, ai2, ai_end, ai_end2;
				boost::graph_traits< Graph >::vertex_iterator vi2, viend2;
				for (boost::tie(ai, ai_end) = boost::adjacent_vertices(v1, g),
					boost::tie(ai2, ai_end2) = boost::adjacent_vertices(v2, g);
					ai != ai_end && ai2 != ai_end2; ai++, ai2++) {
					if (*ai2 == *ai && v1 > v2) {
						//console() << " adjacancyts" << *ai << ',' << *ai2 << " ";
						cycles.push_back({ v1,v2,*ai });

					}
				}
			}
		}
		for (const auto& cycl : convhull) {
			for (size_t i = 0; i < cycl.size(); i++)
			{
				auto v1 = cycl[i];
				auto v2 = cycl[(i + 1) % cycl.size()];
				connectAB(&g, v1, v2, relaxc, 0, true);
			}
		}

		//cycles.insert(cycles.begin(), convhull.begin(), convhull.end());
		addCyclesToVertices(&g, cycles);

	}

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
	ImGui::SetNextItemOpen(true);
	if (ImGui::TreeNode(" Web Settings")) {

		static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
		//ImGui::PushID(0);

		
		ImGui::SliderFloat("density", &density, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("length", &length, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("tension", &tension, 0.0001f, .999f, "%.2f");


		checkforchange(vallist, cachelist);


		//for (int i = 0; i < 7; i++)
		//{

		//	if (i > 0) ImGui::SameLine();
		//	ImGui::PushID(i);
		//	ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 7.0f, 0.5f, 0.5f));
		//	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.5f));
		//	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.5f));
		//	ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 7.0f, 0.9f, 0.9f));
		//	ImGui::VSliderFloat("##v", ImVec2(18, 160), &values[i], 0.0f, 1.0f, "");
		//	ImGui::SliderFloat()
		//	if (ImGui::IsItemActive() || ImGui::IsItemHovered())
		//		ImGui::SetTooltip("%.3f", values[i]);
		//	ImGui::PopStyleColor(4);
		//	ImGui::PopID();
		//}
		
		ImGui::Checkbox("check for forbidden edges", &CHECKFORBIDDEN);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode(" Debug Settings")) {
		ImGui::Text("number of iterations = %i", iterationcounter);
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



		ImGui::TreePop();
	}
	


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


			//for (int i=0; i < convhull.size();i++) {
			//	drawCycleEdges(&g, projection, viewport, convhull, i,Color(0.0f,1.0f,0.0f));
			//}
			//gl::color(0.0f, 0.0f, 1.0f,0.6f);
			//gl::drawLine(position[displayEdgeV_ii], position[displayEdgeV_i]);
			//gl::drawLine(position[displayEdgeV_iii], position[displayEdgeV_iv]);
			//gl::drawLine(position[displayEdgeV_i], position[displayEdgeV_iii]);
			//gl::drawLine(position[displayEdgeV_iv], position[displayEdgeV_ii]);


			
		}

	}
	//			END CAMERA DRAW
}


CINDER_APP( tarantula2App, RendererGl )
