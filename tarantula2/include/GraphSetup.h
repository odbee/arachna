#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderImGui.h"
#include "quickhull/QuickHull.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <udgcd.hpp>
#include <omp.h>

using namespace ci;
using namespace ci::app;
using namespace std;


#define INITIALGRAPHDIRECTORYEXTENSION       "/initialgraph.txt"
#define CYCLESDIRECTORYEXTENSION       "/cycles.txt"
#define VOXELSDIRECTORYEXTENSION       "/voxels.txt"
#define ANCHORPOINTSDIRECTORYEXTENSION       "/anchorpoints.txt"
#define LOGDIRECTORYEXTENSION       "/log.csv"
#define EDGEDATADIRECTORYEXTENSION       "/edges.csv"

#define VERTEXDATADIRECTORYEXTENSION "/vertices.csv"

bool CHECKFORBIDDEN= true;
int GLOBALINT = 1;
int EDGEINDEX=0;
bool RUNRELAX = true;
bool NOTTHESAME=false;
bool CAROUSEL = false;
bool RUNCYCLES = false;




float ALPHA = 0.05f;
size_t CURRENTFRAME = 1;

int	displayCycle_i = 0;
int iterationcounter = 0;

float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
vector<float*> vallist{ &G_density,&G_length,&G_tension };

float G_cachedensity = 0.0005f, G_cachelength = .999f, G_cachetension = 0.0005f;
vector<float*> cachelist{ &G_cachedensity,&G_cachelength,&G_cachetension };




struct EdgeProperties
{
	float restlength;
	float currentlength;
	int index;
	int uniqueIndex;
	bool isforbidden;
	vector<array<float,2>> densityparams;
	float probability;
	float densityval;
};

struct VertexProperties
{
	std::size_t index;
	bool isfixed;
	vec3 pos;
	vec3 movevec;
	std::vector<size_t> cycles;
};

struct GraphProperties {
	int currentindex;
};



typedef boost::adjacency_list<boost::vecS,
	boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
Graph g;


boost::property_map< Graph, vec3 VertexProperties::* >::type position = boost::get(&VertexProperties::pos, g);
boost::property_map< Graph, bool VertexProperties::* >::type fixedBool = boost::get(&VertexProperties::isfixed, g);
boost::property_map< Graph, vec3 VertexProperties::* >::type moveVecPm = boost::get(&VertexProperties::movevec, g);
boost::property_map< Graph, std::vector<size_t> VertexProperties::* >::type cyclesPm = boost::get(&VertexProperties::cycles, g);


boost::property_map< Graph, float EdgeProperties::* >::type currentLengthPm = boost::get(&EdgeProperties::currentlength, g);
boost::property_map< Graph, float EdgeProperties::* >::type densityvalPm = boost::get(&EdgeProperties::densityval, g);

boost::property_map< Graph, float EdgeProperties::* >::type probabilityPm = boost::get(&EdgeProperties::probability, g);

boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);
boost::property_map< Graph, int EdgeProperties::* >::type indexPm = boost::get(&EdgeProperties::index, g);
boost::property_map< Graph, int EdgeProperties::* >::type uniqueIndexPm = boost::get(&EdgeProperties::uniqueIndex, g);

boost::property_map< Graph, bool EdgeProperties::* >::type forbiddenPm = boost::get(&EdgeProperties::isforbidden, g);
boost::property_map< Graph, vector<array<float, 2>> EdgeProperties::* >::type densityPm = boost::get(&EdgeProperties::densityparams, g);



typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_ti;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

edge_ti ei, eiend;
vertex_ti vi, viend;


std::vector<std::vector<size_t>> cycles;
edge_t cache_edge;
vertex_t null = boost::graph_traits<Graph>::null_vertex();
// null edge



struct cyclicvert {
	size_t index;
	vec3 pos;

};

struct cyclicedge {
	edge_ti ei_iterator;
	edge_t descriptor;
	cyclicvert start;
	cyclicvert end;
	vector<size_t> cycles;
	vertex_t divisionvert;
};


std::map<std::array<signed int, 3>, float> voxelMap;
vector<vec3> anchorPoints;

int runAnimation = 0;

bool otherbool = false;
edge_t HOVERED;
edge_t EMPTY;
string EXPORTTITLE;

std::string dirPath = "web";

// GUI PARAMS
float relaxc = 0.8f;
bool hasCycle = false;
bool mDrawVertices = false;
bool mDrawNumbers = false;
bool mDrawVertexInfo = false;
bool colorEdges = false;
bool colorTens = false;
bool colorLength = false;
bool drawNCycle = false;
int x_iterations = 1000;

edge_t highlightedEdge;
bool highlightEdge = false;


struct drawInstance {
	edge_t edge;
	ColorA color;
	size_t time;
};
vector<drawInstance> Instances;

void addDrawInstance(edge_t edge, ColorA color, size_t time) {
	Instances.push_back({ edge,color,time });
}

int UNIQUEINDEX = 1;


struct LogInfo {
	int OldEdgeA, OldEdgeB;
	int NewEdgeA1, NewEdgeA2, NewEdgeB1, NewEdgeB2;
	int NewEdgeC;
	float posA1, posA2;
};

class WebLogger {
private:
	string logfilelocation;
	string edgefilelocation;
	string vertexfilelocation;
	ofstream	myfileof;
	std::ifstream myfileif;
	ofstream edgemyfileof;
	std::ifstream edgemyfileif;

public:
	void initializelocation() {
		logfilelocation = dirPath + LOGDIRECTORYEXTENSION;
		edgefilelocation = dirPath + EDGEDATADIRECTORYEXTENSION;
		vertexfilelocation = dirPath + VERTEXDATADIRECTORYEXTENSION;


		myfileif.open(logfilelocation, std::ifstream::out | std::ifstream::trunc);
		myfileif.close();
		
		myfileof.open(logfilelocation, std::ifstream::out | std::ifstream::trunc);
		myfileof << "OldEdgeA,OldEdgeB, NewEdgeA1,NewEdgeA2,NewEdgeB1,NewEdgeB2,NewEdgeC,posA1,posA2" << endl;
		myfileof.close();

		myfileif.open(edgefilelocation, std::ifstream::out | std::ifstream::trunc);
		myfileif.close();
		myfileof.open(edgefilelocation, std::ios_base::app);
		myfileof << "Unique Index, Index, restlegth, isforbidden(bool), source vertex, target vertex" << endl;
		myfileof.close();


		myfileif.open(vertexfilelocation, std::ifstream::out | std::ifstream::trunc);
		myfileif.close();
		myfileof.open(vertexfilelocation, std::ios_base::app);
		myfileof << "index, isfixed(bool), pos.x,pos.y,pos.z" << endl;
		myfileof.close();



	}

	void addStep(LogInfo log) {
		string in;

		myfileof.open(logfilelocation, std::ios_base::app);
		int OldEdgeA, OldEdgeB;
		int NewEdgeA1, NewEdgeA2, NewEdgeB1, NewEdgeB2;
		int ConnectionEdge;

		myfileof << log.OldEdgeA << "," << log.OldEdgeB << "," << log.NewEdgeA1 << "," << log.NewEdgeA2 << "," << log.NewEdgeB1 << "," << log.NewEdgeB2 <<
			"," << log.NewEdgeC << "," << log.posA1 << "," << log.posA2 << endl;
		myfileof.close();
	}
	void addEdgeLog(edge_t* edge) {
		myfileof.open(edgefilelocation, std::ios_base::app);
		myfileof << g[*edge].uniqueIndex << "," << g[*edge].index << "," << g[*edge].restlength << "," << g[*edge].isforbidden << "," << boost::source(*edge, g) << "," << boost::target(*edge, g) << endl;
		myfileof.close();
	}
	void addVertexLog(vertex_t* vertex) {
		myfileof.open(vertexfilelocation, std::ios_base::app);
		myfileof << *vertex << "," << g[*vertex].isfixed<< "," << g[*vertex].pos.x << "," << g[*vertex].pos.y << "," << g[*vertex].pos.z <<  endl;
		myfileof.close();
	}

	
};

WebLogger webLogger;
