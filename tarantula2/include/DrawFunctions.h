#pragma once
#include "GraphSetup.h"

#include "HelperFunctions.h"


class DrawHandler {
	private:

		gl::BatchRef		mWirePlane;
		gl::BatchRef		mWireCube;
		CameraUi			mCamUi;
		mat4 projection;
		vec4 viewport;
	public:
		mat4 getProjection() {
			return projection;
		}
		vec4 getViewport() {
			return viewport;
		}

		CameraPersp		mCamera;
		double cLamp(double ratio) {
			ratio = std::clamp(ratio, double(0), double(10));
			ratio = ratio / 10;
			return ratio;
		}
		void drawPoints(Graph* g, mat4 proj, vec4 viewp, bool drawNumbers, bool drawVertexPoints, bool drawCycleList) {

			if (drawNumbers) {
				for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
					gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
					vec3 danchorp = position[boost::source(*ei, *g)] + (position[boost::target(*ei, *g)] - position[boost::source(*ei, *g)]) * 0.5f;
					vec2 anchorp1 = glm::project(danchorp, mat4(), proj, viewp);


					//gl::drawString(to_string(cLamp(currentLengthPm[*ei] / restLengthPm[*ei])), anchorp1);
					gl::drawString(to_string(densityvalPm[*ei]), anchorp1);
				}
			}

			if (drawVertexPoints)
			{
				for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {
					//gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
					vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);
					gl::drawSolidCircle(anchorp1, 3);
					//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
				}
			}

			if (drawCycleList)
			{
				for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

					gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
					vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);

					gl::drawString(to_string(*vi), anchorp1, Color::white(), Font("Times New Roman", 20));
					vec2 offset = vec2(0.0f, 22.0f);
					gl::drawString(stringfromCyclesShort(cyclesPm[*vi]), anchorp1 + offset, Color::hex(0xFF0000), Font("Times New Roman", 20));

					//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
				}
			}
		}

		void drawGraph(Graph* g, mat4 proj, vec4 viewp, bool colorEdges = false, bool colorTension = true, bool colorLength = false) {
			gl::ScopedColor color(Color::gray(0.2f));
			gl::color(1.0f, 1.0f, 1.0f, 0.8f);
			for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
				gl::color(1.0f, 1.0f, 1.0f);
				if (colorEdges) {
					gl::color(getColorFromInt(indexPm[*ei]));
				}
				if (colorTension) {
					gl::color(cRamp(currentLengthPm[*ei] / restLengthPm[*ei]));
					console() << cRamp(currentLengthPm[*ei] / restLengthPm[*ei]).r << endl;
				}
				if (colorLength) {
					gl::color(cRamp(currentLengthPm[*ei]));
				}
				if (forbiddenPm[*ei]) {
					gl::color(1.0f, 1.0f, 1.0f, 0.2f);
				}
				else {
					//gl::color(1.0f, 1.0f, 1.0f);
				}
				gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);

			}

		}

		void drawDensityEdges(Graph* g, mat4 proj, vec4 viewp) {
			gl::ScopedColor color(Color::gray(0.2f));
			gl::color(1.0f, 1.0f, 1.0f, 0.8f);
			for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
				//integrateEdge(*ei, *g);

				vec3 p_start = position[boost::source(*ei, *g)];
				vec3 p_end = position[boost::target(*ei, *g)];
				vec3 dir = p_end - p_start;
				int paramslength = densityPm[*ei].size();
				for (size_t i = 0; i < paramslength; i++)
				{
					auto param = (densityPm[*ei])[i];
					auto paramplus = (densityPm[*ei])[(i + 1) % paramslength];
					if (paramplus[0] < 0.001) {
						paramplus[0] = 1;
					}
					gl::color(param[1], 1 - param[1], 1.0f);
					gl::drawLine(p_start + dir * param[0], p_start + dir * paramplus[0]);
					//gl::drawSphere(p_start + dir * param[0],.05f);

				}
			}

		}

		void drawCycle(Graph* g, mat4 proj, vec4 viewp, std::vector<std::vector<size_t>> cycles, int cycleNumber) {
			auto cycle = cycles[cycleNumber % (cycles.size())];
			gl::color(1.0f, 0.0f, 0.0f, 1.0f);
			for (const auto& i : cycle)
			{

				vec2 anchorp1 = glm::project(position[i], mat4(), proj, viewp);
				gl::drawSolidCircle(anchorp1, 5);
			}

		}
		void drawVoxels(std::map<std::array<signed int, 3>, float> grid, float alpha = 1.0f) {
			if (alpha > 0.09) {
				for each (auto vox in grid)
				{
					gl::color(vox.second, 1.0f, 1.0f - vox.second, alpha);
					gl::drawStrokedCube({ vox.first[0],vox.first[1],vox.first[2] }, { 0.9f,0.9f ,0.9f });
				}
			}
		}



		void drawVoxelsBatch(std::map<std::array<signed int, 3>, float> grid, float alpha = 1.0f) {
			gl::BatchRef& Instance = mWireCube;
			if (alpha > 0.09) {
				for each (auto vox in grid)
				{
					if (vox.second > 0.3f || vox.second < 0.01) {
						gl::color(vox.second, 1.0f, 1.0f - vox.second, alpha);
						gl::pushModelMatrix();
						gl::translate({ vox.first[0],vox.first[1],vox.first[2] });
						gl::scale({ 0.9f,0.9f,0.9f });

						Instance->draw();
						gl::popModelMatrix();
						//gl::drawStrokedCube({ vox.first[0],vox.first[1],vox.first[2] }, { 0.9f,0.9f ,0.9f });
					}
				}
			}
		}



		void drawClosestVoxel(int index, Graph& g) {
			//console() << "edgeindex is now " << index << endl;
			int counter = 0;
			edge_t edge;
			for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
				if (counter <= index) {
					edge = *ei;
				}

				counter++;
			}

			vec3 p_start = position[boost::source(edge, g)];
			vec3 p_end = position[boost::target(edge, g)];
			vec3 dir = p_end - p_start;

			float param = 0;
			vector<std::array<float, 2>> params;


			coordcont xcont = { p_start.x,p_end.x, {1,0,0} };	xcont.calcvals();
			coordcont ycont = { p_start.y,p_end.y, {0,1,0} };	ycont.calcvals();
			coordcont zcont = { p_start.z,p_end.z, {0,0,1} };	zcont.calcvals();

			vec3 currvox = { xcont.startvoxel,ycont.startvoxel,zcont.startvoxel };
			gl::color(1.0f, 0.0f, 0.0f);


			gl::drawStrokedCube({ xcont.startvoxel, ycont.startvoxel, zcont.startvoxel }, { 0.9f,0.9f ,0.9f });


		}

		void drawVoxelLine(int index, Graph& g) {
			//console() << "edgeindex is now " << index << endl;
			int counter = 0;
			edge_t edge;
			for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
				if (counter <= index) {
					edge = *ei;
				}

				counter++;
			}

			vec3 p_start = position[boost::source(edge, g)];
			vec3 p_end = position[boost::target(edge, g)];
			vec3 dir = p_end - p_start;

			float param = 0;
			vector<std::array<float, 2>> params;


			coordcont xcont = { p_start.x,p_end.x, {1,0,0} };	xcont.calcvals();
			coordcont ycont = { p_start.y,p_end.y, {0,1,0} };	ycont.calcvals();
			coordcont zcont = { p_start.z,p_end.z, {0,0,1} };	zcont.calcvals();

			vec3 currvox = { xcont.startvoxel,ycont.startvoxel,zcont.startvoxel };
			vector < coordcont > vecofconts = { xcont, ycont, zcont };
			std::sort(vecofconts.begin(), vecofconts.end(), sortbystartvoxeldist);

			checkIfInVoxelMap((int)xcont.startvoxel, (int)ycont.startvoxel, (int)zcont.startvoxel);
			float voxe = voxelMap[{(int)xcont.startvoxel, (int)ycont.startvoxel, (int)zcont.startvoxel}];
			params.push_back({ 0.0f,voxe });

			for (int i = 0; i < max({ xcont.sign * xcont.direction, ycont.sign * ycont.direction, zcont.sign * zcont.direction }); i++)
			{
				for (auto s_voxel : vecofconts) {
					s_voxel.addparam(i, params, currvox);

				}
				gl::drawStrokedCube(currvox, { 0.9f,0.9f ,0.9f });

			}
			gl::color(1.0f, 0.0f, 0.0f);

		}



		void drawCycleEdges(Graph* g, mat4 proj, vec4 viewp, std::vector<std::vector<size_t>> cycles, int cycleNumber, Color col = Color(1.0f, 0.0f, 0.0f)) {

			auto cycle = cycles[cycleNumber % (cycles.size())];
			gl::color(col);
			int j;
			for (int i = 0; i < cycle.size(); i++) {
				j = (i + 1) % cycle.size();

				gl::drawLine(position[cycle[i]], position[cycle[j]]);
			}
		}



		void drawEdgeManagerUpdate() {
			vector< vector<drawInstance>::iterator> removableEdges;
			for (vector<drawInstance>::iterator inst = Instances.begin(); inst != Instances.end(); ++inst) {
				if (inst->time > 0) {
					inst->time--;

				} if (inst->time == 0) {
					removableEdges.push_back(inst);
				}
			}
			
			for (int i = removableEdges.size(); i != 0; i--) {
				Instances.erase(removableEdges[i-1]);
			}
			//for (auto index : removableEdges)
				
		}
		void drawEdgeManagerDraw() {
			
			for (vector<drawInstance>::iterator inst = Instances.begin(); inst != Instances.end(); ++inst) {
				drawSelectedEdge(&g, inst->edge, inst->color);
			}
		}



		void drawSelectedEdge
		(Graph* g, edge_t edg, ColorA col = Color(1.0f, 0.0f, 0.0f)) {
			gl::color(col);
			gl::drawLine(position[boost::source(edg, *g)], position[boost::target(edg, *g)]);

		}

		void drawEdges(Graph* g, vector<edge_t> edges, ColorA col = ColorA(1.0f, 0.0f, 0.0f)) {
			gl::color(col);
			for (const auto& edg : edges)
				gl::drawLine(position[boost::source(edg, *g)], position[boost::target(edg, *g)]);

		}


		void setupCamera() {
			mCamera.lookAt(vec3(30.0f, 20.0f, 40.0f), vec3(0));
			mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
			mCamUi = CameraUi(&mCamera, getWindow());
			auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
			auto colorShader = gl::getStockShader(gl::ShaderDef().color());
			mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(11)).subdivisions(ivec2(10)), colorShader);
			mWireCube = gl::Batch::create(geom::WireCube(), colorShader);
		}

		void setProjectionViewport(){
			mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
			int w = getWindowWidth();
			int h = getWindowHeight();
			vec4 viewport = vec4(0, h, w, -h); // vertical flip is required

		}
		void drawCamera() {
			
			//if (drawNCycle)
			//	drawHandler.drawCycle(&g, projection, viewport, cycles, displayCycle_i);
			gl::ScopedMatrices push;
			gl::setMatrices(mCamera);
		}
};

