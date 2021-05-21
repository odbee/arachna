#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class spidertwoApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void draw() override;
	vec2 linecp(vec2 start, vec2 end, vec2 point);
	vec2 follower;
	vec2 follower2= vec2(-100.0f,-40.0f);
	vec2 magnetp;
	vector<vec2> mPoints;
	vector<array<int,2>> vertIndices;
	int vind=0;
	bool dragged=false;
	bool rdragged = false;

	int minlineindex = 0;
	vec2 cachepos1;
	vec2 cachepos2;
};

void spidertwoApp::setup()
{
}

void spidertwoApp::mouseDown( MouseEvent event )
{
	if(event.isLeft())
	{
		mPoints.push_back(event.getPos());
	}

	if (event.isRight())
	{
		follower = event.getPos();
		float curdist = 100000000000000.0f;
		vec2 closestp;
		int counter = 0;
		
		for (const std::array<int, 2>&arr : vertIndices)
		{
			vec2 distp=linecp(mPoints[arr[0]], mPoints[arr[1]],follower);
			float dist = distance(distp, follower);
			if (dist < curdist)
			{
				curdist = dist;
				closestp = distp;
				minlineindex = counter;
			}
			counter++;
		}
		cachepos1 = mPoints[vertIndices[minlineindex][0]];
		cachepos2 = mPoints[vertIndices[minlineindex][1]];

		mPoints.push_back(closestp);
		int cacheint= vertIndices[minlineindex][1];
		vertIndices[minlineindex][1] = vind;
		vertIndices.push_back({ vind,cacheint });
		

	}

}

void spidertwoApp::mouseDrag(MouseEvent event)
{
	if (event.isLeftDown())
	{
		follower=event.getPos();
		dragged = true;
		//console() << dragged << "  " << follower <<endl;
	}

	if (event.isRightDown())
	{
		follower = event.getPos();
		dragged = true;
		rdragged = true;
		mPoints[vind] = linecp(cachepos1, cachepos2, follower);
		mPoints[vind] = mPoints[vind] + (follower - mPoints[vind]) * 0.2f;

		float curdist = 100000000000000.0f;
		vec2 closestp;
		int counter = 0;
		for (const std::array<int, 2>&arr : vertIndices)
		{
			//if (counter >= vertIndices.size()) {
			//	break;
			//}
			vec2 distp = linecp(mPoints[arr[0]], mPoints[arr[1]], follower);
			float dist = distance(distp, follower);
			if (dist < curdist)
			{
				curdist = dist;
				closestp = distp;
				minlineindex = counter;
			}
			counter++;
		}
		follower2 = closestp;
		//mPoints[vind-1] = event.getPos();
		//console() << dragged << "  " << follower <<endl;
	}


}


void spidertwoApp::mouseUp(MouseEvent event)
{
	dragged = false;
	rdragged = false;
	if (event.isLeft())
	{
		mPoints.push_back(event.getPos());
		vertIndices.push_back({ vind,vind+1 });
		vind+=2;
	}

	if (event.isRight())
	{
		//mPoints.push_back(event.getPos());
		vertIndices.push_back({ vind,vind + 1 });
		mPoints.push_back(follower);\
		vind += 2;

		int cacheint = vertIndices[minlineindex][1];
		vertIndices[minlineindex][1] = vind-1;
		vertIndices.push_back({ vind-1,cacheint });
		//vind++;
	}
}

void spidertwoApp::update()
{
}

void spidertwoApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::color(1.0f, 1.f, 1.0f, 1.0f);
	for (const vec2& point : mPoints) {
		gl::drawSolidCircle(point, 2.0f);
	}
	gl::color(1.0f, 1.f, 1.0f, 1.0f);

	for (const std::array<int, 2>&arr : vertIndices) {
		gl::drawLine(mPoints[arr[0]], mPoints[arr[1]]);
	}
	gl::color(1.0f, 1.0f, 1.0f, 0.4f);
	if (dragged == true) {
		gl::drawSolidCircle(follower, 4.0f);
		
		gl::drawLine(mPoints[vind], follower);
		if (vertIndices.empty() == false&&rdragged==true) {
			gl::drawSolidCircle(follower2, 15.0f);
			gl::drawLine(mPoints[vertIndices[minlineindex][0]], follower);
			gl::drawLine(mPoints[vertIndices[minlineindex][1]], follower);
			//console() << minlineindex << endl;
		}
	}
}

vec2 spidertwoApp::linecp(vec2 start, vec2 end, vec2 point) {
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


CINDER_APP( spidertwoApp, RendererGl )
