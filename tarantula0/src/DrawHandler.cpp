#include "DrawHandler.h"



void DrawHandler::setup() {

}




void DrawHandler::drawPoints( bool drawNumbers, bool drawVertexPoints, bool drawCycleList) {
	
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
		//if (forbiddenPm[*ei]) {
		//	gl::color(1.0f, 1.0f, 1.0f, 0.2f);
		//}
		//else {
		//	//gl::color(1.0f, 1.0f, 1.0f);
		//}
		ci::gl::drawLine(g[boost::source(*ei, g)].pos, g[boost::target(*ei, g)].pos);

	}
}

void DrawHandler::drawGraph( bool colorEdges, bool colorTension, bool colorLength ) {

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
	ci::mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
	int w = ci::app::getWindowWidth();
	int h = ci::app::getWindowHeight();
	ci::vec4 viewport = ci::vec4(0, h, w, -h); // vertical flip is required


	ci::gl::ScopedMatrices push;
	ci::gl::setMatrices(mCamera);

}

void DrawHandler::setupCamera() {
	{
		mCamera.lookAt(ci::vec3(30.0f, 20.0f, 40.0f), ci::vec3(0));
		mCamera.setPerspective(40.0f, ci::app::getWindowAspectRatio(), 0.01f, 1000.0f);
		mCamUi = ci::CameraUi(&mCamera, ci::app::getWindow());
		auto lambertShader =ci::gl::getStockShader(ci::gl::ShaderDef().color().lambert());
		auto colorShader = ci::gl::getStockShader(ci::gl::ShaderDef().color());
		mWirePlane = ci::gl::Batch::create(ci::geom::WirePlane().size(ci::vec2(11)).subdivisions(ci::ivec2(10)), colorShader);
		mWireCube = ci::gl::Batch::create(ci::geom::WireCube(), colorShader);
	}
}

void DrawHandler::drawNthCycle() {
	if (data.highlightCycle&&!data.cycs.empty()&&data.nthCycle<data.cycs.size()) {
		auto cycle = data.cycs[data.nthCycle];
		ci::gl::color(ci::Color(1.0f, 0.0f, 0.0f));
		int j;
		for (int i = 0; i < cycle.size(); i++) {
			j = (i + 1) % cycle.size();

			ci::gl::drawLine(g[cycle[i]].pos, g[cycle[j]].pos);
		}
	}
}

void DrawHandler::drawNthEdge() {
	if (data.highlightEdge) {
		ci::gl::color(ci::Color(1.0f, 0.0f, 0.0f));

		ci::gl::drawLine(g[boost::source(data.highlightedEdge, g)].pos, g[boost::target(data.highlightedEdge, g)].pos);
	}
}