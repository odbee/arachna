#include "DrawHandler.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"

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
		//if (forbiddenPm[*ei]) {
		//	gl::color(1.0f, 1.0f, 1.0f, 0.2f);
		//}
		//else {
		//	//gl::color(1.0f, 1.0f, 1.0f);
		//}
		ci::gl::drawLine(g[boost::source(*ei, g)].pos, g[boost::target(*ei, g)].pos);

	}
}
