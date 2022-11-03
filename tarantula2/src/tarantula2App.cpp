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
#include "PlotEdges.h"
#include "Resetter.h"
#include "CycleImportFunctions.h"
#include "CachedData.h"
using namespace ci;
using namespace ci::app;
using namespace std;



class tarantula2App : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;

	void tarantula2App::runxiterations(int x, Graph& g, float relaxc, std::vector<std::vector<size_t>>& cycles);
	CameraPersp		mCamera;
private:
	gl::BatchRef		mWirePlane;
	gl::BatchRef		mWireCube;
	CameraUi			mCamUi;
	float relaxc = 0.4f;
	bool hasCycle = false;
	bool mDrawVertices = false;
	bool mDrawNumbers = false;
	bool mDrawVertexInfo = false;
	bool colorEdges = false;
	bool colorTens = false;
	bool colorLength = false;

	bool drawNCycle = false;

	int x_iterations = 1000;

	std::pair<edge_t, bool> c, d;
	vec3 camPos = { 30.0f, 10.0f, 40.0f };
	string profilername = "profiler.csv";
	ofstream profile_out;
	int cachediter;
	IniHandler iniHand;


	//needed vars for animate function

	cyclicedge N_startedge, N_goaledge;	edge_ti N_graphstart, N_graphend; edge_ti N_ei_startEdge; edge_t N_ed_startEdge;
	std::vector<size_t> N_edgeinds;
	int N_seconditer;	edge_t N_ed_goalEdge; edge_ti N_ei_goalEdge;
	vector<size_t> N_currCyc;
	vector<edge_t> N_connedges;



};

void tarantula2App::setup()
{

	EXPORTTITLE = "positions.obj";

	Color h;
	iniHand.initImGui("cached_ui.ini");

	//console() << ImGui::ColorConvertU32ToFloat4( ImGui::GetColorU32(41)).x 
	//	<< "   "
	//	<< ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(41)).y 
	//	<< "   "
	//	<< ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(41)).z << endl;
	//			CAMERA SETUP
	{
		mCamera.lookAt(vec3(30.0f, 20.0f, 40.0f), vec3(0));
		mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
		mCamUi = CameraUi(&mCamera, getWindow());
		auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
		auto colorShader = gl::getStockShader(gl::ShaderDef().color());
		mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(11)).subdivisions(ivec2(10)), colorShader);
		mWireCube = gl::Batch::create(geom::WireCube(), colorShader);
	}
	//			END CAMERA SETUP


	ImGui::Initialize();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	//io.Fonts->AddFontFromFileTTF("../resources/fonts/Abordage-Regular.ttf", 14);
	io.Fonts->AddFontFromFileTTF("../resources/fonts/Karrik-Regular.ttf", 15);

	//ImGui::GetStyle().WindowRounding = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().WindowPadding.x = 30.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().WindowPadding.y = 20.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().FramePadding.y = 5.0f;// <- Set this on init or use ImGui::PushStyleVar()
	ImGui::GetStyle().FramePadding.x = 8.0f;// <- Set this on init or use ImGui::PushStyleVar()

	ImGui::GetStyle().ChildRounding = 0.0f;
	ImGui::GetStyle().FrameRounding = 5.0f;
	ImGui::GetStyle().GrabRounding = 20.0f;
	ImGui::GetStyle().PopupRounding = 0.0f;
	ImGui::GetStyle().ScrollbarRounding = 12.0f;
	ImGui::GetStyle().WindowBorderSize = 2.0f;


	ImGui::GetStyle().FrameBorderSize = 0.0f;
	ImGui::GetStyle().ChildBorderSize = 0.0f;
	ImGui::GetStyle().GrabMinSize = 20.0f;

	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);



	initVoxelMap(VOXELFILENAME);
	initAnchorPoints(ANCHORPOINTSFILENAME);

	console() << voxelMap.size() << endl;
	console() << "voxel map initiated successfully, value of {0,0,0} is:" << voxelMap[{0, 0, 0}] << endl;
	//InitialWebFromPc(&g,relaxc, "txtf.txt");
	InitialWebFromObj(&g, relaxc, OBJFILENAME, cycles);


	std::ofstream ofs;

	profile_out.open(profilername, std::ios_base::trunc);
	profile_out << "counter,command,duration" << endl;
	profile_out.close();

	//addcyclesfromPc(relaxc, g, cycles);
}

void tarantula2App::keyDown(KeyEvent event)
{
	if (event.getCode() == 99) { // "c"
		//addRandomEdge(&g, relaxc);
		auto start = std::chrono::high_resolution_clock::now();
		addRandomCyclicEdgeTesting(&g, relaxc, &cycles);
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
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
		//exportGraph(g);
		exportGraphOBJ(g, dirPath+EXPORTTITLE);
	}
	if (event.getChar() == 'x') {

		console() << voxelMap.size() << endl;

	}

	if (event.getChar() == 'a') {
		runAnimation = 1;

	}

	//console() << event.getCode() << endl;
}
vec3 rotateVec(float alpha, vec3 vector) {
	return { vector.x * cos(alpha) - vector.z * sin(alpha)
		,vector.y,
		vector.x * sin(alpha) + vector.z * cos(alpha) };
}

void tarantula2App::update()
{
	CURRENTFRAME++;
	auto start = std::chrono::high_resolution_clock::now();
	if (RUNRELAX && !runAnimation)
		relaxPhysics(&g);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	if (cachediter != iterationcounter) {
		profile_out.open(profilername, std::ios_base::app);
		profile_out << iterationcounter << "," << "physics" << "," << duration.count() << endl;
		profile_out.close();

	}
	if (CAROUSEL) {
		camPos = rotateVec(0.01f, camPos);
		mCamera.lookAt(camPos, vec3(0));
	}
	if (RUNCYCLES && !(CURRENTFRAME % 30)) {
		displayCycle_i++;
	}
	cachediter = iterationcounter;
	if (displayCycle_i >= cycles.size()) {
		displayCycle_i = 0;
	}


	if (runAnimation) {
		// TODO FIX THE ISSUE WHEN THERE ARE NO AVAILABLE EDEGES
		if (runAnimation == 1) {
			console() << "fiding first edge" << endl;


			tie(ei, eiend) = boost::edges(g); //need this, will get error otherwise

			tie(N_graphstart, N_graphend) = boost::edges(g);

			N_ed_startEdge = getRandomEdgeFromEdgeListIntegrated(&g, N_graphstart, N_graphend, true);
			EDGEADDEDINT = 1;
			WHICHEDGE = N_ed_startEdge;
		}
		 
		if (runAnimation == 50) {
			console() << "fiding second edge" << endl;
			initEdge(N_ed_startEdge, N_startedge, g);
			N_connedges = getConnectedEdges(&g, N_startedge, &cycles, N_edgeinds);
			CONNEDGES = N_connedges;
		}


		if (runAnimation == 100) {
			if (N_connedges.size()) {


				N_ed_goalEdge = getRandomEdgeFromEdgeListIntegrated(&g, N_connedges.begin(), N_connedges.end());
				size_t cycleIndex = *(N_edgeinds.begin() + std::distance(N_connedges.begin(), std::find(N_connedges.begin(), N_connedges.end(), N_ed_goalEdge)));


				initEdge(N_ed_goalEdge, N_goaledge, g);
				displayEdgeV_iii = N_goaledge.start.index;
				displayEdgeV_iv = N_goaledge.end.index;

				vector<size_t> currCyc = cycles[cycleIndex];
				updatetext(to_string(counter) + "c");
				displayCycle_i = cycleIndex;
				shiftCycle(currCyc, N_startedge.start.index, N_startedge.end.index);

				auto find1 = std::find(currCyc.begin(), currCyc.end(), N_goaledge.start.index);
				auto find2 = std::find(currCyc.begin(), currCyc.end(), N_goaledge.end.index);
				auto gei1 = std::distance(currCyc.begin(), find1);
				auto gei2 = std::distance(currCyc.begin(), find2);
				auto gei = (gei1 < gei2) ? find2 : find1;
				// get vetex indices

				vector<size_t> left(currCyc.begin(), gei);
				vector<size_t> right(gei, currCyc.end());

				N_startedge.divisionvert = boost::add_vertex(g); fixedBool[N_startedge.divisionvert] = false;
				N_goaledge.divisionvert = boost::add_vertex(g); fixedBool[N_goaledge.divisionvert] = false;
				//created 2 points
				//getDivPoint(startedge.descriptor);
				position[N_startedge.divisionvert] = interpolate(N_startedge, getDivPoint(N_startedge.descriptor));
				position[N_goaledge.divisionvert] = interpolate(N_goaledge, getDivPoint(N_startedge.descriptor));
				//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
				cycles[cycleIndex] = left;
				cycles.push_back(right);
				size_t lastindex = cycles.size() - 1;
				displayCycle_ii = lastindex;
				resolveIntersections(N_startedge, N_goaledge, cycleIndex, lastindex, g, cycles);

			}
			else {
				console() << " no cycles found, returning" << endl;
				return;
			}

			NEWEDGES = connectEdges(&g, N_startedge, N_goaledge, relaxc);
			counter++;
			updatetext("\n");
		}
		runAnimation++;
		if (runAnimation == 150)
		{
			runAnimation = 0;
		}

	}




}

using namespace ImGui;
void tarantula2App::runxiterations(int x, Graph& g, float relaxc, std::vector<std::vector<size_t>>& cycles) {
	for (size_t i = 0; i < x; i++)
	{
		addRandomCyclicEdgeTesting(&g, relaxc, &cycles);
		for (size_t j = 0; j < 7; j++)
		{
			relaxPhysics(&g);
		}
		iterationcounter++;
	}

}
void tarantula2App::draw()
{
	bool isopen;
	ImGui::Begin("STUFS", &isopen, /*ImGuiWindowFlags_NoTitleBar +*/ ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize);

	//	static float xs[51], ys1[51], ys2[51];
	//for (int i = 0; i < 51; ++i) {
	//	xs[i] = i * 0.02;
	//	ys1[i] = 1.0 + 0.5 * sin(25 * xs[i]) * cos(2 * xs[i]);
	//	ys2[i] = 0.5 + 0.25 * sin(10 * xs[i]) * sin(xs[i]);
	//}
	//ImGui::PlotLines("frames", xs, IM_ARRAYSIZE(xs));
	//PlotEdges("", ys1, xs, IM_ARRAYSIZE(xs), 0, 0, 0, 1, { 600,300 });
	//
	tie(ei, eiend) = boost::edges(g);
	auto num_edges = std::distance(ei, eiend);
	vector<float> lengths(num_edges);
	vector<float> tensions(num_edges);
	int  indekx = 0;

	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

		//console() << currentLengthPm[*ei] << endl;
		lengths[indekx] = currentLengthPm[*ei];

		tensions[indekx] = currentLengthPm[*ei] / restLengthPm[*ei];
		//console() << tensions[indekx] << endl;
		indekx++;

		//console() << to_string(tensionCols[indekx]) << endl;

		//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << to_string(position[boost::target(*ei, g)]) << ")" << "]" << endl;
		//if (!forbiddenPm[*ei]) {
		//	myfile << "[" << "{" << to_string(position[boost::source(*ei, g)]) << "};{" << to_string(position[boost::target(*ei, g)]) << "}" << "]" << endl;
		//}
	}
	//console() << lengths[32] << endl;

	//PlotEdges2("framess", lengths, tensions, 0, 0, FLT_MAX, { 600,300 });
	PlotGraphEdges("framess", g, 0, 0, FLT_MAX, { 600,300 });

	ImGui::End();
	ImGui::Begin("Params", &isopen, ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoBackground + ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoResize);

	//ImGui::StyleColorsLight();


	if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("number of iterations = %i", iterationcounter);



		ImGui::SliderFloat("density", &G_density, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("length", &G_length, 0.0001f, .999f, "%.2f");
		ImGui::SliderFloat("tension", &G_tension, 0.0001f, .999f, "%.2f");

		ImGui::InputInt("x iterations", &x_iterations, 50, 200);
		if (ImGui::Button("run x iterations")) {
			runxiterations(x_iterations, g, relaxc, cycles);

		}


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
		ImGui::InputInt("edge number", &EDGEINDEX);
		if (ImGui::InputText("import directory path ", &dirPath)) {
			iniHand.overwriteTagImGui("graph directory", dirPath);
		}
		
		OBJFILENAME = dirPath + INITIALGRAPHDIRECTORYEXTENSION;
		CYCLESFILENAME = dirPath + CYCLESDIRECTORYEXTENSION;
		VOXELFILENAME = dirPath + VOXELSDIRECTORYEXTENSION;
		ANCHORPOINTSFILENAME = dirPath + ANCHORPOINTSDIRECTORYEXTENSION;
		ImGui::InputText("export filename: ", &EXPORTTITLE);
		//if (displayCycle_i >= cycles.size()) {
		//	displayCycle_i = displayCycle_i % cycles.size();
		//}
		if (ImGui::Button("Load Graph")) {
			InitialWebFromObj(&g, relaxc, OBJFILENAME, cycles);

		}
		if (ImGui::Button("Load Cycles")) {
			loadcycles(&g, cycles, CYCLESFILENAME);
			addCyclesToVertices(&g, cycles);
		}

		ImGui::Checkbox("Draw Vertices", &mDrawVertices);

		ImGui::SliderFloat("Alpha Value", &ALPHA, 0.0001f, .999f, "%.2f");


		ImGui::Checkbox("RELAX", &RUNRELAX);
		ImGui::Checkbox("Carousel", &CAROUSEL);
		ImGui::Checkbox("Run Through Cycles", &RUNCYCLES);

		ImGui::Checkbox("Draw Edge Numbers", &mDrawNumbers);
		ImGui::Checkbox("Draw Vertex Info", &mDrawVertexInfo);

		ImGui::Checkbox("highlight Nth Cycle", &drawNCycle);
		ImGui::Checkbox("Color Edge Indices", &colorEdges);
		ImGui::Checkbox("Color Edge Tensions", &colorTens);
		ImGui::Checkbox("Color Edge Lengths", &colorLength);

	}
	ImGui::End();



	gl::clear(Color(0, 0, 0));
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


		//drawClosestVoxel(EDGEINDEX,g);
		//drawVoxelLine(EDGEINDEX, g);
		{
			//gl::color(1.0f, 1.0f, 1.0f, 0.1f);
			//gl::pushModelMatrix();
			//gl::translate({ 0,-5.5 });
			//for (int i = 0; i <= 11; ++i) {
			//	mWirePlane->draw();
			//	gl::translate({ 0,1 });
			//}
			//gl::popModelMatrix();

			//gl::pushModelMatrix();
			//gl::rotate(_Pi/2);
			//gl::translate({ 0,-5.5 });
			//for (int i = 0; i <= 11; ++i) {
			//	mWirePlane->draw();
			//	gl::translate({ 0,1 });
			//}
			//gl::popModelMatrix();
			drawVoxelsBatch(voxelMap, mWireCube, ALPHA);
			drawGraph(&g, projection, viewport, colorEdges, colorTens, colorLength);
			if (drawNCycle) {
				drawCycleEdges(&g, projection, viewport, cycles, displayCycle_i);
				//drawCycleEdges(&g, projection, viewport, cycles, displayCycle_ii, Color(1.0f,0.6f,0.0f));
			}
			//drawDensityEdges(&g, projection, viewport);

			if (drawNCycle) {
				drawCycleEdges(&g, projection, viewport, cycles, displayCycle_i);
				//drawCycleEdges(&g, projection, viewport, cycles, displayCycle_ii, Color(1.0f,0.6f,0.0f));
			}
			//drawDensityEdges(&g, projection, viewport);
			if (runAnimation > 0 && runAnimation <= 100)
				drawSelectedEdge(&g, WHICHEDGE, { 0.5f,0.5f,0.82f,0.8f });

			if (runAnimation > 50) {
				drawEdges(&g, N_connedges, { 1.0f, 0.5f, 0.0f,0.7f });
			}
			if (runAnimation > 100) {
				drawEdges(&g, NEWEDGES, { 0.5f,0.5f,0.82f,0.8f });
				drawSelectedEdge(&g, NEWEDGES[0], { 0.65f,0.45f,0.82f,0.8f });

			}
			if (HOVERED != EMPTY) {
				drawSelectedEdge(&g, HOVERED, { 1.0f,0.5f,0.2f,0.8f });

			}
		}

	}
	//			END CAMERA DRAW
}


CINDER_APP(tarantula2App, RendererGl)
