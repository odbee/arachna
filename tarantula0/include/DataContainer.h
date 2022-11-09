#pragma once
#include <vector>
#include <string>
#include <map>
#include <array>

#include "cinder/app/App.h"
#include "GraphSetup.h"
#include "cinder/CinderImGui.h"
#include "cinder/CameraUi.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include "HelperFunctions.h"

class DataContainer
{
	private:
		void setup();
		edge_ti ei, eiend;
		vertex_ti vi, viend;
		edge_t cache_edge;
		vertex_t nullVert;
		struct coordcont;
		struct cyclicvert {
			size_t index;
			ci::vec3 pos;

		};
		struct cyclicedge {
			edge_ti ei_iterator;
			edge_t descriptor;
			cyclicvert start;
			cyclicvert end;
			std::vector<size_t> cycles;
			vertex_t divisionvert;
		};
	public:
		DataContainer() { setup(); }




		voxel_map voxels;
		anchor_points anchorPoints;
		Graph g;
		cycleList cycles;
		std::string initialGraphDE, cyclesDE, voxelsDE, anchorPDE, fullPath;
		bool forbiddenCheck, runRelax, carousel, runCycles;
		bool highlightCycle;
		int nthCycle;
		int GLOBALINT;
		int edgeIndex;
		float alpha;
		edge_t highlightedEdge;
		bool highlightEdge;
		float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
		float G_cachedensity = 0.0005f, G_cachelength = .999f, G_cachetension = 0.0005f;
		std::vector<float*> vallist{ &G_density,&G_length,&G_tension };
		std::vector<float*> cachelist{ &G_cachedensity,&G_cachelength,&G_cachetension };

		void updateDirectories();
		void loadCycles();
		void addCyclesToVertices();
		edge_t connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind = 0, bool isforbidden = false);
		void emptyCyclesFromVertices();
		void exportGraph(std::string directory);
		void exportGraphOBJ(Graph g, std::string directory);
		float getDivPoint(edge_t edge);
		std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g);
		template <typename T>
		edge_t getRandomEdgeFromEdgeListIntegrated(T& begin, T& end, bool forbcheck, float density, float length, float tension);
		float integrateEdge(edge_t edge, voxel_map voxelMap);
		void InitialWebFromObj(float rc, std::string filename);
		void addIntersectionVertexToCycles(cyclicedge& edge, int cycleIndex);

		void addRandomCyclicEdgeTesting(float rc);

		std::vector<edge_t> connectEdges(cyclicedge startedge, cyclicedge goaledge, float rc);
		std::vector<edge_t> getConnectedEdges(cyclicedge edge, std::vector<size_t>& edgeindices, bool forbCheck);
		void initEdge(edge_t ed, cyclicedge& cedge);
		ci::vec3 interpolate(cyclicedge edge, float t);
		void removebyindex(std::vector<size_t>& vec, size_t ind);
		void resolveIntersections(cyclicedge start, cyclicedge goal, size_t cycle1index, size_t cycle2index);
		void shiftCycle(std::vector<size_t>& cycle, int index1, int  index2);
		void updatetext(std::string in);
		std::vector<size_t> vectorcycles(std::vector<size_t> vec);
		void relaxPhysics();
		edge_t pickrandomEdge(edge_ti& begin, edge_ti& end);

        struct ExampleAppLog
		{
			ImGuiTextBuffer     Buf;
			ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
			bool                AutoScroll;  // Keep scrolling if already at the bottom.
			ExampleAppLog()
			{
				AutoScroll = true;
				Clear();
			}
			void    Clear();
			void    AddLog(const char* fmt, ...) IM_FMTARGS(2);
			void    Draw(const char* title, bool* p_open = NULL);
		};

        ExampleAppLog my_log;


		ci::mat4 proj;
		ci::vec4 viewp;
		ci::CameraPersp		mCamera;

		ci::gl::BatchRef		mWirePlane;
		ci::gl::BatchRef		mWireCube;
		ci::CameraUi			mCamUi;
		void setupCamera();
		void drawCamera();
		void drawPoints(bool drawNumbers, bool drawVertexPoints, bool drawCycleList);
		void drawGraph(bool colorEdges = false, bool colorTension = true, bool colorLength = false);
		void drawNthCycle();
		void drawNthEdge();


};


struct DataContainer::coordcont {
	float start;
	float end;
	ci::vec3 axis;
	signed int sign;
	int startvoxel;
	int endvoxel;
	float startvoxeldist;
	float direction;

	void calcvals();
	bool compareendvoxels();
	void addparam(int index, std::vector<std::array<float, 2>>& paramlist, ci::vec3& currentvoxel, voxel_map vh);
	bool sortbystartvoxeldist(const coordcont& x, const coordcont& y);
	bool operator < (const DataContainer::coordcont& str) const
	{
		return (startvoxeldist < str.startvoxeldist);
	}
};


