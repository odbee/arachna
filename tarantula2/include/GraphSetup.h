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

bool CHECKFORBIDDEN= true;
int GLOBALINT = 1;
int EDGEINDEX=0;
bool RUNRELAX = true;
bool NOTTHESAME=false;
bool CAROUSEL = true;
bool RUNCYCLES = false;




float ALPHA = 0.15f;
size_t CURRENTFRAME = 1;

int	displayCycle_i = 0;
int	displayCycle_ii = 0;
int displayEdgeV_i=0;
int displayEdgeV_ii=0;
int displayEdgeV_iii = 0;
int displayEdgeV_iv = 0;
int iterationcounter = 0;
string VERTEXFILENAME = "";
string VOXELFILENAME = "";
string OBJFILENAME = "";
vector<size_t> vertlist;

float G_density = 0.0005f, G_length = .999f, G_tension = 0.0005f;
vector<float*> vallist{ &G_density,&G_length,&G_tension };

float G_cachedensity = 0.0005f, G_cachelength = .999f, G_cachetension = 0.0005f;
vector<float*> cachelist{ &G_cachedensity,&G_cachelength,&G_cachetension };




struct EdgeProperties
{
	float restlength;
	float currentlength;
	int index;
	bool isforbidden;
	vector<array<float,2>> densityparams;
	float probability;
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
boost::property_map< Graph, float EdgeProperties::* >::type probabilityPm = boost::get(&EdgeProperties::probability, g);

boost::property_map< Graph, float EdgeProperties::* >::type restLengthPm = boost::get(&EdgeProperties::restlength, g);
boost::property_map< Graph, int EdgeProperties::* >::type indexPm = boost::get(&EdgeProperties::index, g);

boost::property_map< Graph, bool EdgeProperties::* >::type forbiddenPm = boost::get(&EdgeProperties::isforbidden, g);
boost::property_map< Graph, vector<array<float, 2>> EdgeProperties::* >::type densityPm = boost::get(&EdgeProperties::densityparams, g);


boost::graph_traits<Graph>::edge_iterator ei, eiend;
boost::graph_traits< Graph >::vertex_iterator vi, viend;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;

typedef Graph::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::edge_iterator edge_ti;

std::vector<std::vector<size_t>> cycles;
edge_t cache_edge;
boost::graph_traits<Graph>::vertex_descriptor null = boost::graph_traits<Graph>::null_vertex();
// null edge




std::map<std::array<signed int, 3>, float> voxelMap;


bool EDGEADDED = false;
edge_t WHICHEDGE;
int EDGEADDEDINT = 0;
int runAnimation = 0;
vector<edge_t> CONNEDGES;
vector<edge_t> NEWEDGES;
vector<int> CONNEDGESGRAPH;
bool otherbool = false;
edge_t HOVERED;
edge_t EMPTY;