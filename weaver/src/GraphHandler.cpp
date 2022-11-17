#include "GraphHandler.h"

void GraphHandler::setup() {
	nullVert = boost::graph_traits<Graph>::null_vertex();
}

void GraphHandler::addEdgeFromRecipe(int index) {
	// add new vertices -
	// connect new vertices to edge -
	// set length of new edge, taken from edges csv -
	// conncet new vertices with previus edges -
	// 
	// locate the indices of the edges that replace the divided edge.
	// locate them by theor indices.
	// 
	// 
	//
	RecipeContainer& recipe = recipeInfo[index];
	EdgeContainer ENC = edgeInfo[recipe.NewEdgeC];
	EdgeContainer ENA1 = edgeInfo[recipe.NewEdgeA1];
	EdgeContainer ENA2 = edgeInfo[recipe.NewEdgeA2];
	EdgeContainer ENB1 = edgeInfo[recipe.NewEdgeB1];
	EdgeContainer ENB2 = edgeInfo[recipe.NewEdgeB2];
	EdgeContainer EOA = edgeInfo[recipe.OldEdgeA];
	EdgeContainer EOB = edgeInfo[recipe.OldEdgeB];
	//
	
	auto a = boost::add_vertex(g);
	g[a].isfixed = false;
	g[a].pos = { 0,0,0 };
	auto b = boost::add_vertex(g);
	g[b].isfixed = false;
	g[b].pos = { 0,0,0 };
	edge_t edgeC = boost::add_edge(a, b, g).first;
	
	edge_t edgeA1 = boost::add_edge(a, EOA.sourceV, g).first;
	edge_t edgeA2 = boost::add_edge(a, EOA.targetV, g).first;
	edge_t edgeB1 = boost::add_edge(b, EOB.sourceV, g).first;
	edge_t edgeB2 = boost::add_edge(b, EOB.targetV, g).first;
	
	updateEdge(edgeC, ENC);
	updateEdge(edgeA1, ENA1);
	updateEdge(edgeA2, ENA2);
	updateEdge(edgeB1, ENB1);
	updateEdge(edgeB2, ENB2);

	EdgeContainer EA = edgeInfo[recipe.OldEdgeA];
	EdgeContainer EB = edgeInfo[recipe.OldEdgeB];	
	boost::remove_edge(EA.sourceV, EA.targetV, g);
	boost::remove_edge(EB.sourceV, EB.targetV, g);

}

void GraphHandler::updateEdge( edge_t edge_desc, EdgeContainer edge_cont) {
	g[edge_desc].restlength = edge_cont.restlength;
	g[edge_desc].uniqueIndex = edge_cont.uniqueIndex;
	g[edge_desc].isforbidden = false;
	g[edge_desc].index = edge_cont.index;


}


std::vector<EdgeContainer> GraphHandler::loadEdges(std::string fname) {
	std::vector<EdgeContainer> content;
	std::string row [5];
	std::string line, word;

	std::fstream file(fname, std::ios::in);
	int countA = 0;
	int countB = 0;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (countB != 0) {
				countA = 0;

				std::stringstream str(line);

				while (getline(str, word, ','))
					row[countA++] = (word);
				content.push_back(EdgeContainer{ std::stoi(row[0]),std::stoi(row[1]) ,std::stof(row[2]) ,std::stoi(row[3]) ,std::stoi(row[4]) });
			}
			
			countB++;
		}
	}
	edgeInfo = content;
	ci::app::console() << "added " << edgeInfo.size() << "edges" << std::endl;
	return content;
}


std::vector<RecipeContainer> GraphHandler::loadRecipe(std::string fname) {
	std::vector<RecipeContainer> content;
	std::string row[9];
	std::string line, word;
	
	std::fstream file(fname, std::ios::in);
	int countA = 0;
	int countB = 0;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (countB != 0) {
				countA = 0;

				std::stringstream str(line);

				while (getline(str, word, ','))
					row[countA++] = (word);
				content.push_back(RecipeContainer{ std::stoi(row[0]),std::stoi(row[1]) ,
													std::stoi(row[2]),std::stoi(row[3]) ,
													std::stoi(row[4]),std::stoi(row[5]) ,
													std::stoi(row[6]),
													std::stof(row[7]) ,std::stof(row[8]) });
			}

			countB++;
		}
	}
	recipeInfo = content;
	ci::app::console() << "added " << recipeInfo.size() << "steps" << std::endl;
	return content;
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