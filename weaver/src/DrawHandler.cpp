#include "DrawHandler.h"



void DrawHandler::setup() {

}

void DrawHandler::drawPoints() {
	ci::vec2 anchorp1;
	boost::graph_traits<Graph>::out_edge_iterator oei, oeiend;
	std::vector<ci::vec2> anchorpoints;
	for (std::tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
		std::tie(oei, oeiend) = boost::out_edges(*vi,g);

		if (oei!=oeiend) 
		{
			anchorp1 = glm::project(g[*vi].pos, ci::mat4(), proj, viewp);
			while (std::find(anchorpoints.begin(), anchorpoints.end(), anchorp1) != anchorpoints.end()) {
				anchorp1 =anchorp1+ ci::vec2{25, 0};
			}
			anchorpoints.push_back(anchorp1);
			
			ci::gl::drawString(std::to_string(*vi), anchorp1, ci::Color::white(), ci::Font("Arial", 25));
		}
		
	}
}



void DrawHandler::drawDivisionPoints(EdgesGraph& edgesGraph, std::map<int, EdgeContainer>& edgeMap) {
	std::vector<float>* DivEdgeLengths;
	float sumOfAllLengths = 0;
	int outEdgeIndex;
	ci::vec3 startpoint, endpoint;
	float interpoint;
	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		interpoint = 0;
		//DivEdgeLengths = &edgesGraph[g[*ei].uniqueIndex].DivEdgeLengths;
		DivEdgeLengths = &edgesGraph[g[*ei].uniqueIndex-1].interPts;
		startpoint = g[boost::source(*ei, g)].pos;
		endpoint = g[boost::target(*ei, g)].pos;
		//edgesGraph[g[*ei].uniqueIndex];
		//sumOfAllLengths = std::accumulate(DivEdgeLengths->begin(), DivEdgeLengths->end(), (float)0);
		
		for (auto iter : edgesGraph[g[*ei].uniqueIndex-1].interPts) {
			ci::gl::color(ci::ColorA(0.0f, 1.0f, 0.0f, 0.4f));
			interpoint += iter;
			if (interpoint<0.9999)
				ci::gl::drawSphere(startpoint + (endpoint - startpoint) * (interpoint) ,0.05);
		}
	}
}




void DrawHandler::drawGraph(bool colorEdges, bool colorTension, bool colorLength) {

	ci::gl::ScopedColor color(ci::Color::gray(0.2f));
	ci::gl::color(1.0f, 1.0f, 1.0f, 0.8f);
	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		ci::gl::color(1.0f, 1.0f, 1.0f);
		//if (colorEdges) {
		//	gl::color(getColorFromInt(indexPm[*ei]));
		//}
		//if (colorTension) {
		//	gl::color(cRamp(currentLengthPm[*ei] / restLengthPm[*ei]));
		//	console() << cRamp(currentLengthPm[*ei] / restLengthPm[*ei]).r << endl;
		//}
		//if (colorLength) {
		//	gl::color(cRamp(currentLengthPm[*ei]));
		//}
		if (g[*ei].isforbidden) {
			ci::gl::color(1.0f, 1.0f, 1.0f, 0.2f);
		}
		//else {
		//	//gl::color(1.0f, 1.0f, 1.0f);
		//}
		ci::gl::drawLine(g[boost::source(*ei, g)].pos, g[boost::target(*ei, g)].pos);

	}
}

void DrawHandler::drawCamera() {
	proj = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
	int w = ci::app::getWindowWidth();
	int h = ci::app::getWindowHeight();
	viewp = ci::vec4(0, h, w, -h); // vertical flip is required



}

void DrawHandler::setupCamera() {
	{
		mCamera.lookAt(ci::vec3(30.0f, 20.0f, 40.0f), ci::vec3(0));
		mCamera.setPerspective(40.0f, ci::app::getWindowAspectRatio(), 0.01f, 1000.0f);
		mCamUi = ci::AdamsCameraUi(data,&mCamera, ci::app::getWindow());
		auto lambertShader = ci::gl::getStockShader(ci::gl::ShaderDef().color().lambert());
		auto colorShader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
		mWirePlane = ci::gl::Batch::create(ci::geom::WirePlane().size(ci::vec2(11)).subdivisions(ci::ivec2(10)), colorShader);
		mWireCube = ci::gl::Batch::create(ci::geom::WireCube(), colorShader);
	}
}


void DrawHandler::drawNthEdge() {

		ci::gl::color(ci::Color(0.0f, 1.0f, 0.0f));

		ci::gl::drawLine(g[boost::source(data.hovered_edge, g)].pos, g[boost::target(data.hovered_edge, g)].pos);

}


void DrawHandler::drawSelectedEdge() {
	if (data.selected_edge != data.empty_edge) {
		ci::gl::color(ci::Color(1.0f, 0.0f, 0.0f));
		ci::gl::drawLine(g[boost::source(data.selected_edge, g)].pos, g[boost::target(data.selected_edge, g)].pos);
	}
	

}

ci::Ray DrawHandler::calculateRay(ci::vec2 mousePos) {
	float u = mousePos.x / (float)ci::app::getWindowWidth();
	float v = mousePos.y / (float)ci::app::getWindowHeight();
	ci::Ray ray = mCamera.generateRay(u, 1.0f - v, mCamera.getAspectRatio());
	return ray;
}

void DrawHandler::mouseDown(ci::app::MouseEvent& event) {

	ci::app::console() << "clicking old item" << std::endl;

}