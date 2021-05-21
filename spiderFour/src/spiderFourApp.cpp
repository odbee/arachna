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


	//saved arrays
	vector<vec2> wPoints;
	vector<array<int, 2>> edgeIndices;
	


	//exttra functions
	string list_edges(vector<array<int, 2>> edgelist);
	string list_vertices(vector<vec2> vertexlist);
	vec2 linecp(vec2 start, vec2 end, vec2 point);
	pair<int, vec2> closestedge(vec2 mousepos, vector<array<int, 2>> edgelist, vector<vec2> veclist);
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
		refdist = 100000000.0f; //reset distance
		vec2 closestp;			// check for closest point

		int counter = 0;		//count to keep track of which edge

		// look for the closest edge
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		closestEdgeIndex = result.first;
		closestp = result.second;
		console() << closestp[0] << endl;
		// cache closest edge point position
		closestEdgePointPos1 = wPoints[edgeIndices[closestEdgeIndex][0]];
		closestEdgePointPos2 = wPoints[edgeIndices[closestEdgeIndex][1]];
		wPoints.push_back(closestp);
		edgeIndices.push_back({ (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex][1] });
		edgeIndices[closestEdgeIndex][1] = (int)wPoints.size() - 1;
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
		wPoints.push_back(mousefollower);
		edgeIndices.push_back({ (int)wPoints.size() - 2,(int)wPoints.size() - 1 });
		edgeIndices.push_back({ (int)wPoints.size() - 1, edgeIndices[closestEdgeIndex2][1] });
		edgeIndices[closestEdgeIndex2][1] = (int)wPoints.size() - 1;
		r_dragged = false;

	}
	console() << list_edges(edgeIndices) << endl;
	console() << list_vertices(wPoints) << wPoints.size() <<  endl;
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
		pair<int, vec2> result = closestedge(mousefollower, edgeIndices, wPoints);
		closestEdgeIndex2=result.first;

		secondfollower = result.second;

	}
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

CINDER_APP( spiderFourApp, RendererGl )
