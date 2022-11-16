#include "GraphHandler.h"
#include <fstream>

void GraphHandler::setup() {
	nullVert = boost::graph_traits<Graph>::null_vertex();
}


edge_t GraphHandler::connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind, bool isforbidden) {
	float dd = distance(g[endPointA].pos, g[endPointB].pos);

	edge_t edge = boost::add_edge(endPointA, endPointB, g).first;
	g[edge].currentlength = dd;
	g[edge].restlength = dd * rc;
	g[edge].isforbidden = isforbidden;
	g[edge].index = ind;

	return edge;
}




std::optional<edge_t> GraphHandler::GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g)
{
	for (auto e : boost::make_iterator_range(out_edges(v, g))) {
		if (target(e, g) == u)
			return e;
	}
	return {};
	throw std::domain_error("my_find_edge: not found");
}





void GraphHandler::InitialWebFromObj(float rc, std::string filename) {
	bool hasCycle;
	std::ifstream MyReadFile(filename);
	std::string line;
	int numberOfVertices = 0;
	data.my_log.AddLog("[info] initializing starting graph.\n");
	if (MyReadFile.good()) {
		while (getline(MyReadFile, line)) {
			std::stringstream ss(line);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			if (!vstrings.empty()) {
				if (vstrings[0] == "v") {
					auto a = boost::add_vertex(g);

					numberOfVertices++;
					g[a].pos = { stof(vstrings[1]),stof(vstrings[2]),stof(vstrings[3]) };
					if (vstrings.size() >= 5) {
						g[a].isfixed = stof(vstrings[4]); //
					}
					else {
						g[a].isfixed = true;
					}



				}
				if (vstrings[0] == "l") {
					edge_t e = connectAB(stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, rc);
					g[e].isforbidden = stoi(vstrings[3]);
				}
			}

		}

		data.my_log.AddLog("added %i vertices \n", numberOfVertices);
	}
	else {
		data.my_log.AddLog("[warning] Initial Graph not found. check in your directory.\n");
	}

}
//
//ci::vec3 GraphHandler::interpolate(cyclicedge edge, float t) {
//	return edge.start.pos + (edge.end.pos - edge.start.pos) * t;
//}


void GraphHandler::relaxPhysics() {
	float k = 1.1f;
	float eps = 0.1f;
	Graph::vertex_descriptor v1, v2;
	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);

		if (length(g[v2].pos - g[v1].pos) > 0.01) {
			if (!g[v1].isfixed) {
				g[v1].movevec += 1 * k * (g[*ei].currentlength - g[*ei].restlength) *
					normalize(g[v2].pos - g[v1].pos);
			}
			if (!g[v2].isfixed) {
				g[v2].movevec += 1 * k * (g[*ei].currentlength - g[*ei].restlength) *
					normalize(g[v1].pos - g[v2].pos);
			}
		}
	}

	for (std::tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

		//position[*vi] += moveVecPm[*vi];
		g[*vi].pos += eps * g[*vi].movevec;
		g[*vi].movevec = ci::vec3(0, 0, 0);
	}
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		g[*ei].currentlength = distance(g[v1].pos, g[v2].pos);
	}
}