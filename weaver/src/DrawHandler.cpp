#include "DrawHandler.h"



void DrawHandler::setup() {

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