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
	DrawHandler drawHandler;
	ImGuiHandler imGuiHandler;

	cyclicedge A_goaledge, A_startedge;
	std::vector<edge_t> A_connectableEdges;
	std::vector<size_t>A_edgeinds;
	WebLogger webLogger;

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
	webLogger.initializelocation();
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
		
		LogInfo log= addRandomCyclicEdgeTesting(&g, relaxc, &cycles);
		webLogger.addStep(log);
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
		addDrawInstance(highlightedEdge, { 1.0f,0.0f,0.0f,1.0f }, 30);
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


	addRandomCyclicEdgeAnimated(&g, relaxc, &cycles, runAnimation,A_startedge,A_goaledge,A_connectableEdges,A_edgeinds);
	if (runAnimation == 90) {
		getConnectableEdges(&g, A_startedge, &cycles, A_edgeinds, false);
		//console() << g[A_startedge.descriptor].isforbidden << endl;
	}

	drawHandler.drawEdgeManagerUpdate();
	if (runAnimation) {
		console() << runAnimation << endl;
		
		runAnimation++;
	}
	

}

using namespace ImGui;

void tarantula2App::draw()
{
	imGuiHandler.drawEdgesDiagram();
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
			if (HOVERED != EMPTY) {
				drawHandler.drawSelectedEdge(&g, HOVERED, { 1.0f,0.5f,0.2f,0.8f });

			}
			//if (highlightEdge) {
			//	//drawHandler.addDrawInstance(highlightedEdge, { 1.0f,0.0f,0.0f,1.0f },30);
			//	/*drawHandler.drawSelectedEdge(&g, highlightedEdge, { 1.0f,0.0f,0.0f,1.0f });*/

			//}
			drawHandler.drawEdgeManagerDraw();
		}

	}
	//			END CAMERA DRAW
}


CINDER_APP(tarantula2App, RendererGl)
