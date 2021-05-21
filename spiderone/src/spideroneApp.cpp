#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class spideroneApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	vec2 linecp(vec2 start, vec2 end, vec2 point);


	vec2 p1 = vec2(-300, 0)+((vec2)getWindowPos())*0.5f;
	vec2 p2 = vec2(300, 0) + ((vec2)getWindowPos()) * 0.5f;
	vec2 pc;
	vec2 follower;
	vec2 sxn=p1;
};

void spideroneApp::setup()
{
}

void spideroneApp::mouseDrag( MouseEvent event )
{
	follower = event.getPos();
	sxn = linecp(p1, p2, follower);
	sxn = sxn + (follower- sxn) * 0.1f;
}

void spideroneApp::update()
{
}

void spideroneApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::drawLine(p1, sxn);
	gl::drawLine(p2, sxn);
	gl::drawLine(follower, sxn);
	

	gl::drawSolidCircle(sxn, 3.1f);
	gl::drawSolidCircle(follower, 3.1f);
}


vec2 spideroneApp::linecp(vec2 start, vec2 end, vec2 point) {
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


CINDER_APP( spideroneApp, RendererGl )
