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
	vec2 cachedEdgePoint;

	//
	vec2 beampt1;
	vec2 beampt2;

	struct edge {
		array<int, 2> endpts;
		int start;
		int end;
		float tension=1;
	};
	struct pt {
		vec2 pos;
		vector<int> adjacentedges;
	};
	//saved arrays
	vector<pt> wPoints;
	vector<edge> edgeIndices;
	



	//exttra functions
	string list_edges(vector<edge> edgelist);
	string list_vertices(vector<pt> vertexlist);
	vec2 linecp(vec2 start, vec2 end, vec2 point);
	pair<int, vec2> closestedge(vec2 mousepos, vector<edge> edgelist, vector<pt> veclist);
	pair<vec2, vec2> line_line_pts(vec2 a0, vec2 a1, vec2 b0, vec2 b1);
	pair<vector<pt>, vector<edge>> animate(vector<pt> vertexlist, vector<edge> edgelist) {
		vector<pt> copyverts = vertexlist;
		int counter = 0;
		for (const pt& point : vertexlist) {

			//copyverts[counter]=point+
			counter++;
		}
		return make_pair(vertexlist, edgelist);
	};
};

void spiderFourApp::setup()
{
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
		vec2 closestp;			// check for closest point

		// look for the closest edge
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		closestEdgeIndex = result.first;
		closestp = result.second;
		console() << closestp[0] << endl;
		cachedEdgePoint= closestp;
		
		// cache closest edge point position
		closestEdgePointPos1 = wPoints[edgeIndices[closestEdgeIndex].endpts[0]].pos;
		closestEdgePointPos2 = wPoints[edgeIndices[closestEdgeIndex].endpts[1]].pos;

		// add the closestpoint to the vertices 
		wPoints.push_back({ closestp });
		//connect new point to old edge 1
		edgeIndices.push_back(edge{ { (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex].endpts[1]} ,1 });
		wPoints[(int)wPoints.size() - 1].adjacentedges.push_back(edgeIndices.size() - 1);
		wPoints[edgeIndices[closestEdgeIndex].endpts[1]].adjacentedges.push_back(edgeIndices.size() - 1);
		// replace other edge with new point
		edgeIndices[closestEdgeIndex].endpts[1] = (int)wPoints.size() - 1;

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
		wPoints.back().pos= cachedEdgePoint;
		//move point towards mouse
		wPoints.back().pos += (mousefollower - wPoints.back().pos) * 0.2f;
		//look for closest edge
		
		
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		float restlength1 = length(cachedEdgePoint - closestEdgePointPos1);
		float restlength2 = length(cachedEdgePoint - closestEdgePointPos2);

		float tenselength1 = length(wPoints.back().pos - closestEdgePointPos1);
		float tenselength2 = length(wPoints.back().pos - closestEdgePointPos2);

		edgeIndices[closestEdgeIndex].tension =  tenselength1 /restlength1;
		edgeIndices[edgeIndices.size() - 1].tension = tenselength2 /restlength2;

		closestEdgeIndex2=result.first;
		secondfollower = result.second;

		//pair<vec2, vec2> res= line_line_pts(closestEdgePointPos1,closestEdgePointPos2,wPoints[edgeIndices[closestEdgeIndex2][0]], wPoints[edgeIndices[closestEdgeIndex2][1]]);

		//beampt1 = res.first;
		//beampt2 = res.second;

		

	}
}

void spiderFourApp::mouseUp(MouseEvent event)
{
	if (event.isLeft())
	{
		wPoints.push_back({ event.getPos() });
		int wps = wPoints.size();
		edgeIndices.push_back(edge{ {wps - 2,wps - 1} });
		l_dragged = false;
	}

	if (event.isRight()) {
		//add mouse point to list
		wPoints.push_back({ secondfollower });
		wPoints.back().pos = wPoints.back().pos + (wPoints[(int)wPoints.size() - 2].pos-wPoints.back().pos)*0.2f;
		// connect closest edge to mouse point
		edgeIndices.push_back(edge{{ (int)wPoints.size() - 2,(int)wPoints.size() - 1 }});
		edgeIndices.push_back(edge{ { (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex2].endpts[1] } });

		float restlength1 = length(secondfollower - wPoints[edgeIndices[closestEdgeIndex2].endpts[0]].pos);
		float restlength2 = length(secondfollower - wPoints[edgeIndices[closestEdgeIndex2].endpts[1]].pos);

		float tenselength1 = length(wPoints.back().pos - wPoints[edgeIndices[closestEdgeIndex2].endpts[0]].pos);
		float tenselength2 = length(wPoints.back().pos - wPoints[edgeIndices[closestEdgeIndex2].endpts[1]].pos);

		edgeIndices[closestEdgeIndex2].tension = tenselength1/ restlength1;
		edgeIndices[edgeIndices.size() - 1].tension = tenselength2/restlength2;

		
		edgeIndices[closestEdgeIndex2].endpts[1] = (int)wPoints.size() - 1;
		r_dragged = false;

	}
	console() << list_edges(edgeIndices) << endl;
	//console() << list_vertices(wPoints) << wPoints.size() <<  endl;
}


void spiderFourApp::update()
{
	if (!(l_dragged && r_dragged)) {
		animate(wPoints, edgeIndices);
	}
}

void spiderFourApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(1.0f, 1.f, 1.0f, 1.0f);
	//draw points
	for (const pt& point : wPoints) {
		gl::drawSolidCircle(point.pos, 2.0f);
		gl::drawString(to_string((int)point.pos[0]) + "," + to_string((int)point.pos[1]), point.pos);
	}
	//draw edges
	for (const edge&arr : edgeIndices) {
		gl::drawLine(wPoints[arr.endpts[0]].pos, wPoints[arr.endpts[1]].pos);

		vec2 cpoint = wPoints[arr.endpts[0]].pos + (wPoints[arr.endpts[1]].pos - wPoints[arr.endpts[0]].pos) * 0.5f;
		gl::drawString(to_string(arr.tension), cpoint);

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
		if (!edgeIndices.empty()) {
			gl::drawLine(wPoints[edgeIndices[closestEdgeIndex2].endpts[0]].pos, mousefollower);
			gl::drawLine(wPoints[edgeIndices[closestEdgeIndex2].endpts[1]].pos, mousefollower);
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

pair<int, vec2> spiderFourApp::closestedge(vec2 mousepos, vector<edge> edgelist, vector<pt> veclist) {
	float curdist = 100000000000000.0f;
	vec2 closestp;
	int counter = 0;
	int referencer;
	for (const edge&arr : edgelist)
	{
		vec2 distp = linecp(veclist[arr.endpts[0]].pos, veclist[arr.endpts[1]].pos, mousepos);
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

string spiderFourApp::list_edges(vector<edge> edgelist) {
	ostringstream oss;
	
	oss << "list of edges: ";
	oss << "{";
	for (const edge&arr : edgelist) {
		
		oss << "[" << arr.endpts[0] << "," << arr.endpts[1] << "]" << "," << arr.tension << "   ";
		/*outstring += (arr[0]);
		outstring += ",";
		outstring += arr[1];
		outstring += "]";*/

	
	}
	oss << "}" << endl;
	//outstring += "\n";
	return oss.str();
}

string spiderFourApp::list_vertices(vector<pt> vertexlist) {
	ostringstream oss;
	oss << "list of vertices: ";
	for (const pt& arr : vertexlist) {

		oss << "[" << arr.pos[0] << "," << arr.pos[1]<< "]" << "  ";
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
