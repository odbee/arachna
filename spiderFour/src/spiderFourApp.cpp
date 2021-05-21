#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

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

		//reference distance
	float refdist = 100000000.0f;
	int closestEdgeIndex;
	int closestEdgeIndex2;

		//
	vec2 closestEdgePointPos1;
	vec2 closestEdgePointPos2;

	//
	vec2 beampt1;
	vec2 beampt2;

	//saved arrays
	vector<vec2> wPoints;
	vector<array<int, 2>> edgeIndices;
	


	//exttra functions
	string list_edges(vector<array<int, 2>> edgelist);
	string list_vertices(vector<vec2> vertexlist);
	vec2 linecp(vec2 start, vec2 end, vec2 point);
	pair<int, vec2> closestedge(vec2 mousepos, vector<array<int, 2>> edgelist, vector<vec2> veclist);
	pair<vec2, vec2> line_line_pts(vec2 a0, vec2 a1, vec2 b0, vec2 b1);
};

void spiderFourApp::setup()
{
}

void spiderFourApp::mouseDown( MouseEvent event )
{
	// add point when click left
	if (event.isLeft())
	{
		wPoints.push_back(event.getPos());
	}

	// connect two edges when push right
	if (event.isRight()) {
		// follow the mouse
		mousefollower = event.getPos();
		vec2 closestp;			// check for closest point

		// look for the closest edge
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		closestEdgeIndex = result.first;
		closestp = result.second;
		console() << closestp[0] << endl;


		// cache closest edge point position
		closestEdgePointPos1 = wPoints[edgeIndices[closestEdgeIndex][0]];
		closestEdgePointPos2 = wPoints[edgeIndices[closestEdgeIndex][1]];

		// add the closestpoint to the vertices and connect the edge to it
		wPoints.push_back(closestp);
		edgeIndices.push_back({ (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex][1] });
		edgeIndices[closestEdgeIndex][1] = (int)wPoints.size() - 1;
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
		//reset point position every frame
		wPoints.back()= linecp(closestEdgePointPos1, closestEdgePointPos2, mousefollower);
		//move point towards mouse
		wPoints.back() += (mousefollower - wPoints.back()) * 0.2f;
		//look for closest edge
		
		
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		
		closestEdgeIndex2=result.first;
		secondfollower = result.second;

		pair<vec2, vec2> res= line_line_pts(closestEdgePointPos1,closestEdgePointPos2,wPoints[edgeIndices[closestEdgeIndex2][0]], wPoints[edgeIndices[closestEdgeIndex2][1]]);

		beampt1 = res.first;
		beampt2 = res.second;
	}
}

void spiderFourApp::mouseUp(MouseEvent event)
{
	if (event.isLeft())
	{
		wPoints.push_back(event.getPos());
		int wps = wPoints.size();
		edgeIndices.push_back({ wps-2,wps-1});
		l_dragged = false;
	}

	if (event.isRight()) {
		//add mouse point to list
		wPoints.push_back(mousefollower);
		// connect closest edge to mouse point
		edgeIndices.push_back({ (int)wPoints.size() - 2,(int)wPoints.size() - 1 });
		edgeIndices.push_back({ (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex2][1] });
		edgeIndices[closestEdgeIndex2][1] = (int)wPoints.size() - 1;
		r_dragged = false;

	}
	console() << list_edges(edgeIndices) << endl;
	console() << list_vertices(wPoints) << wPoints.size() <<  endl;
}


void spiderFourApp::update()
{
	//console() << "mosh" << "  " << "mosh" << endl;
}

void spiderFourApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(1.0f, 1.f, 1.0f, 1.0f);
	//draw points
	for (const vec2& point : wPoints) {
		gl::drawSolidCircle(point, 2.0f);
	}
	//draw edges
	for (const std::array<int, 2>&arr : edgeIndices) {
		gl::drawLine(wPoints[arr[0]], wPoints[arr[1]]);
	}

	//realtime draw
	gl::color(1.0f, 1.0f, 1.0f, 0.4f);
	if (l_dragged) {
		gl::drawSolidCircle(mousefollower, 4.0f);
		gl::drawLine(wPoints.back(), mousefollower);
	}
	if (r_dragged) {
		gl::drawSolidCircle(secondfollower, 4.0f);
		gl::drawLine(mousefollower, wPoints.back());
		if (!edgeIndices.empty()) {
			gl::drawLine(wPoints[edgeIndices[closestEdgeIndex2][0]], mousefollower);
			gl::drawLine(wPoints[edgeIndices[closestEdgeIndex2][1]], mousefollower);
		}

		gl::color(0.0f, 1.0f, 1.0f, 1.0f);
		gl::drawSolidCircle(beampt1, 8.0f);
		gl::drawSolidCircle(beampt2, 8.0f);

	}
}

vec2 spiderFourApp::linecp(vec2 start, vec2 end, vec2 point) {
	vec2 ab = end - start;
	vec2 ap = point - start;
	float t = dot(ab, ap) / (length(ab) * length(ab));
	if (t > 1.0f)
	{
		return end;
	}
	else if (t < 0.0f)
	{
		return start;
	}
	else	return  start + t * ab;
}

pair<int, vec2> spiderFourApp::closestedge(vec2 mousepos, vector<array<int, 2>> edgelist, vector<vec2> veclist) {
	float curdist = 100000000000000.0f;
	vec2 closestp;
	int counter = 0;
	int referencer;
	for (const std::array<int, 2>&arr : edgelist)
	{
		vec2 distp = linecp(veclist[arr[0]], veclist[arr[1]], mousepos);
		float dist = distance(distp, mousepos);
		if (dist < curdist)
		{
			curdist = dist;
			closestp = distp;
			referencer = counter;
		}
		counter++;
	}

	return make_pair( referencer , closestp);

}

string spiderFourApp::list_edges(vector<array<int, 2>> edgelist) {
	ostringstream oss;
	
	oss << "list of edges: ";
	for (const std::array<int, 2>&arr : edgelist) {
		
		oss <<  "[" << arr[0] << "," << arr[1] << "]" << "  ";
		/*outstring += (arr[0]);
		outstring += ",";
		outstring += arr[1];
		outstring += "]";*/

	
	}
	oss << endl;
	//outstring += "\n";
	return oss.str();
}

string spiderFourApp::list_vertices(vector<vec2> vertexlist) {
	ostringstream oss;
	oss << "list of vertices: ";
	for (const vec2& arr : vertexlist) {

		oss << "[" << arr[0] << "," << arr[1] << "]" << "  ";
	}
	oss << endl;

	return oss.str();
}

pair<vec2, vec2> spiderFourApp::line_line_pts(vec2 a0, vec2 a1, vec2 b0, vec2 b1) {
	float SMALL_NUM = 0.00000000001f;

	vec2 u = a1 - a0;
	vec2 v = b1 - b0;
	vec2 w = a0 - b0;
	float    a = dot(u, u);         // always >= 0
	float    b = dot(u, v);
	float    c = dot(v, v);         // always >= 0
	float    d = dot(u, w);
	float    e = dot(v, w);
	float    D = a * c - b * b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

		// compute the line parameters of the two closest points
	if (D < SMALL_NUM) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
	tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

	vec2 A = a0 + sc * u;
	vec2 B = b0 + tc * v;

	return make_pair(A, B);

}

CINDER_APP( spiderFourApp, RendererGl )
