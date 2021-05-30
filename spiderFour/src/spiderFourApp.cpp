#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "spiderprimitives.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class spiderFourApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;
	//variables
	bool l_dragged = false;
	bool r_dragged = false;
	vec2 mousefollower=vec2(-100.0f,-40.0f);
	vec2 secondfollower = vec2(-100.0f, -40.0f);
	vec2 cachedEdgePoint;

	//saved arrays
	vector<pt> wPoints;
	edgeList edges;
	
};

void spiderFourApp::setup()
{
	edges.pointlist = &wPoints;
}

void spiderFourApp::mouseDown( MouseEvent event )
{
	// add point when click left
	if (event.isLeft())
	{
		wPoints.push_back({ event.getPos() });
	}

	// connect two edges when push right
	if (event.isRight()) {
		// follow the mouse
		mousefollower = event.getPos();
		edges.ce1i = nClosestEdge(&mousefollower, edges.edgeIndices, wPoints);
		cachedEdgePoint= mousefollower;
		edges.cachedProjectedPt1 = cachedEdgePoint;
		edges.set_cachedpoints(edges.ce1i);
		wPoints.push_back({ mousefollower });
		//connect new point to old edge 1
		edges.nconnect_point_with_edge((int)wPoints.size() - 1, edges.ce1i,edges.cachedProjectedPt1);

	}

}

void spiderFourApp::mouseDrag(MouseEvent event)
{
	if (event.isLeftDown())
	{
		mousefollower = event.getPos();
		l_dragged = true;
	}

	if (event.isRightDown())
	{
		r_dragged = true;
		mousefollower = event.getPos();
		secondfollower = event.getPos();
		wPoints.back().pos= cachedEdgePoint;		//reset point position every frame
		wPoints.back().pos += (mousefollower - wPoints.back().pos) * 0.2f;		//move point towards mouse
		edges.edgeIndices.back().currentlength = distance(wPoints[edges.edgeIndices.back().endpts[1]].pos, wPoints.back().pos);
		edges.edgeIndices[edges.ce1i].currentlength= distance(wPoints[edges.edgeIndices[edges.ce1i].endpts[0]].pos, wPoints.back().pos);


		edges.ce2i = nClosestEdge(&secondfollower, edges.edgeIndices, wPoints);		//look for closest edge
		edges.cachedProjectedPt2 = secondfollower;
		edges.set_cachedpoints(edges.ce2i);
	}
}

void spiderFourApp::mouseUp(MouseEvent event)
{
	if (event.isLeft())
	{
		wPoints.push_back({ event.getPos() });
		edges.push_back((int)wPoints.size() - 2,(int)wPoints.size() - 1);
		l_dragged = false;
	}

	if (event.isRight()) {
		wPoints.push_back({ secondfollower }); 		//add mouse point to list
		wPoints.back().pos = wPoints.back().pos + (wPoints[(int)wPoints.size() - 2].pos-wPoints.back().pos)*0.2f;
		edges.push_back_tense((int)wPoints.size() - 2, (int)wPoints.size() - 1,
			distance(edges.cachedProjectedPt1,edges.cachedProjectedPt2)*0.9f,distance(wPoints.back().pos,wPoints[(int)wPoints.size() - 2].pos)); 		// connect closest edge point to mouse point
		edges.nconnect_point_with_edge((int)wPoints.size() - 1, edges.ce2i,edges.cachedProjectedPt2);
		r_dragged = false;

	}
	console() << list_edges(edges.edgeIndices) << endl;
	//console() << list_vertices(wPoints) << wPoints.size() <<  endl;
}


void spiderFourApp::update()
{
	if (!(l_dragged && r_dragged)) {
		animate(wPoints, edges.edgeIndices);
	}
}

void spiderFourApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(1.0f, 1.f, 1.0f, 1.0f);
	//draw points
	for (const pt& point : wPoints) {
		gl::drawSolidCircle(point.pos, 2.0f);
	}
	//draw edges
	for (const edge&arr : edges.edgeIndices) {
		gl::drawLine(wPoints[arr.endpts[0]].pos, wPoints[arr.endpts[1]].pos);

		vec2 cpoint = wPoints[arr.endpts[0]].pos + (wPoints[arr.endpts[1]].pos - wPoints[arr.endpts[0]].pos) * 0.5f;
		vec2 c2point = wPoints[arr.endpts[0]].pos + (wPoints[arr.endpts[1]].pos - wPoints[arr.endpts[0]].pos) * 0.5f+vec2(0.0f,10.0f);
		vec2 c3point = wPoints[arr.endpts[0]].pos + (wPoints[arr.endpts[1]].pos - wPoints[arr.endpts[0]].pos) * 0.5f + vec2(0.0f, 20.0f);
		gl::drawString(to_string((float)arr.restlength), cpoint);
		gl::drawString(to_string((float)arr.currentlength), c2point);
		gl::drawString(to_string((float)arr.currentlength-arr.restlength), c3point);

	}

	//realtime draw
	gl::color(1.0f, 1.0f, 1.0f, 0.4f);
	if (l_dragged) {
		gl::drawSolidCircle(mousefollower, 4.0f);
		gl::drawLine(wPoints.back().pos, mousefollower);
	}
	if (r_dragged) {
		gl::drawSolidCircle(secondfollower, 4.0f);
		gl::drawLine(mousefollower, wPoints.back().pos);
		if (!edges.edgeIndices.empty()) {
			gl::drawLine(wPoints[edges.edgeIndices[edges.ce2i].endpts[0]].pos, mousefollower);
			gl::drawLine(wPoints[edges.edgeIndices[edges.ce2i].endpts[1]].pos, mousefollower);
		}
		gl::color(0.0f, 1.0f, 1.0f, 1.0f);

	}
}



CINDER_APP( spiderFourApp, RendererGl )
