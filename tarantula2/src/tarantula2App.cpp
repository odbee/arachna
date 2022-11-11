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
#include "ImGuiHandler.h"
using namespace ci;
using namespace ci::app;
using namespace std;



class tarantula2App : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown(KeyEvent event) override;




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
	DrawHandler drawHandler;
	ImGuiHandler imGuiHandler;


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
	drawHandler.setupCamera();
	//			END CAMERA SETUP

	imGuiHandler.setupImGui();



	initWeb(g, relaxc, cycles);

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
		//addRandomCyclicEdgeAnimated(&g, relaxc, &cycles);
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

	if (event.getChar() == 'k') {
		highlightedEdge = findRandomEdge();
		highlightEdge = true;

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
		drawHandler.mCamera.lookAt(camPos, vec3(0));
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
			N_connedges = getConnectableEdges(&g, N_startedge, &cycles, N_edgeinds, CHECKFORBIDDEN);
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

void tarantula2App::draw()
{
	//imGuiHandler.drawEdgesDiagram();
	mat4 projection = drawHandler.mCamera.getProjectionMatrix() * drawHandler.mCamera.getViewMatrix();
	int w = getWindowWidth();
	int h = getWindowHeight();
	vec4 viewport = vec4(0, h, w, -h); // vertical flip is required

	imGuiHandler.drawParametersWindow(iniHand);


	gl::clear(Color(0, 0, 0));
	//gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		drawHandler.setProjectionViewport();
		drawHandler.drawPoints(&g, drawHandler.getProjection(), drawHandler.getViewport(), mDrawNumbers, mDrawVertices, mDrawVertexInfo);
		if (drawNCycle)
			drawHandler.drawCycle(&g, projection, viewport, cycles, displayCycle_i);
		drawHandler.drawCamera();
		gl::ScopedMatrices push;
		gl::setMatrices(drawHandler.mCamera);
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
			drawHandler.drawVoxelsBatch(voxelMap, ALPHA);
			drawHandler.drawGraph(&g, drawHandler.getProjection(), drawHandler.getViewport(), colorEdges, colorTens, colorLength);
			if (drawNCycle) {
				drawHandler.drawCycleEdges(&g, drawHandler.getProjection(), drawHandler.getViewport(), cycles, displayCycle_i);
				//drawCycleEdges(&g, projection, viewport, cycles, displayCycle_ii, Color(1.0f,0.6f,0.0f));
			}
			//drawDensityEdges(&g, projection, viewport);

			//drawHandler.drawDensityEdges(&g, projection, viewport);
			if (runAnimation > 0 && runAnimation <= 100)
				drawHandler.drawSelectedEdge(&g, WHICHEDGE, { 0.5f,0.5f,0.82f,0.8f });

			if (runAnimation > 50) {
				drawHandler.drawEdges(&g, N_connedges, { 1.0f, 0.5f, 0.0f,0.7f });
			}
			if (runAnimation > 100) {
				drawHandler.drawEdges(&g, NEWEDGES, { 0.5f,0.5f,0.82f,0.8f });
				drawHandler.drawSelectedEdge(&g, NEWEDGES[0], { 0.65f,0.45f,0.82f,0.8f });

			}
			if (HOVERED != EMPTY) {
				drawHandler.drawSelectedEdge(&g, HOVERED, { 1.0f,0.5f,0.2f,0.8f });

			}
			if (highlightEdge) {
				drawHandler.drawSelectedEdge(&g, highlightedEdge, { 1.0f,0.0f,0.0f,1.0f });

			}
		}

	}
	//			END CAMERA DRAW
}


CINDER_APP(tarantula2App, RendererGl)
