#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "spiderprimitives.h"



#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
using namespace ci;
using namespace ci::app;
using namespace std;

class spiderboostApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;


	struct EdgeProperties
	{
		float restlength;
		float currentlength;
	};
	struct VertexProperties
	{
		bool isfixed;
		vec2 pos;
	};

	vec2 mousefollower;

	//typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, VertexProperties, EdgeProperties> Graph;
	Graph g;
	//typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
	//typedef std::pair<EdgeIterator, EdgeIterator> EdgePair;
	//typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
	//EdgePair ep;
	//VertexDescriptor u, v;

	size_t startv, endv, start2v, end2v;
	int closestedge1index;
	vec2 cachedEdgePoint;
	vec2 secondfollower = vec2(-100.0f, -40.0f);
};

void spiderboostApp::setup()
{

}

void spiderboostApp::mouseDown( MouseEvent event )
{
	if (event.isLeft()) {
		startv = boost::add_vertex({ false, event.getPos() }, g);
	}

	if (event.isRight()) {
		mousefollower = event.getPos();
		closestedge1index = nClosestEdge(&mousefollower, g);
		cachedEdgePoint = mousefollower;

	}
}

void spiderboostApp::mouseUp(MouseEvent event)
{
	if (event.isLeft())
	{
		endv = boost::add_vertex(g); g[endv].isfixed = true; g[endv].pos = event.getPos();

		float dst = distance(g[startv].pos, g[endv].pos);
		auto e = boost::add_edge(startv, endv, { dst,dst }, g);
	}



}
void spiderboostApp::mouseDrag(MouseEvent event)
{
}


void spiderboostApp::update()
{
}

void spiderboostApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(1.0f, 1.f, 1.0f, 1.0f);
	for (const auto& point : g.m_vertices) {
		gl::drawSolidCircle(point.m_property.pos, 2.0f);
		
	}
	for (ep = edges(g); ep.first != ep.second; ++ep.first)
	{
		// Get the two vertices that are joined by this edge...
		u = source(*ep.first, g);
		v = target(*ep.first, g);
		gl::drawLine(g.m_vertices[u].m_property.pos, g.m_vertices[v].m_property.pos);

	}
	
	//typedef property_map<Graph, vertex_index_t>::type IndexMap;
	//typedef property_map<Graph, >::type IndexMap;

}

CINDER_APP( spiderboostApp, RendererGl )
