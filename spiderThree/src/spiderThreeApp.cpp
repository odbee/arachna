#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class spiderThreeApp : public App {
  public:
	void setup() override;
	void mouseMove(MouseEvent event) override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	vector<vec2> mPoints;
	vector<array<int, 2>> vertIndices;
	void update() override;
	void draw() override;
	vec2 linecp(vec2 start, vec2 end, vec2 point);
	vec2 follower;
	vec2 mousepos;
	bool dragtrigger;
	int minlineindex = 0;
	vec2 cachepos1;
	vec2 cachepos2;
	int vind = 0;
	class vert {
		public:
			vec2 pos;
			vector<int> neighbors;
	};
	struct edge {
		int start;
		int end;
	};

	vector<vert> vertlist;
	vector<edge> edgelist;
};

void spiderThreeApp::setup()
{

}

void spiderThreeApp::mouseDown( MouseEvent event )
{
	dragtrigger = true;
	if (event.isLeft())
	{
		vertlist.push_back(vert{event.getPos() });
	}
	if (event.isRight())
	{
		float curdist = 100000000000000.0f;
		vec2 closestp;
		int counter = 0;

		for (const edge& e : edgelist)
		{
			vec2 distp = linecp(mPoints[e[0]], mPoints[e[1]], mousepos);
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
		int cacheint = vertIndices[minlineindex][1];
		vertIndices[minlineindex][1] = vind;
		vertIndices.push_back({ vind,cacheint });
	}

}

void spiderThreeApp::mouseDrag(MouseEvent event)
{
	mousepos = event.getPos();
	dragtrigger = false;
	if (event.isLeftDown())
	{
		//console() << dragged << "  " << follower <<endl;
	}
}

void spiderThreeApp::mouseUp(MouseEvent event)
{
	if (event.isLeft())
	{
		vertlist.push_back(vert{ event.getPos() });
		if (dragtrigger = true) {
			edgelist.push_back({ (int)vertlist.size()-2,(int)vertlist.size() - 1 });
			vertlist[(int)vertlist.size() - 2].neighbors.push_back((int)vertlist.size() - 1);
		}
	}
}

void spiderThreeApp::mouseMove(MouseEvent event)
{
	mousepos= vec2(-100.0f,100.0f);
}
void spiderThreeApp::update()
{
}

void spiderThreeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::drawSolidCircle(mousepos, 5.0f);
	for (const vert& v : vertlist) {
		gl::drawSolidCircle(v.pos, 2.0f);
		for (const int& index : v.neighbors) {
			gl::drawLine(v.pos, vertlist[index].pos);
		}
	}
	
	console() << mousepos << endl;
	
	//for (const edge&e : edgelist) {
	//	gl::drawLine(vertlist[e.start].pos, vertlist[e.end].pos);
	//	
	//}
	if (vertlist.size() > 0&&!dragtrigger) {
		gl::drawLine(mousepos, vertlist[vertlist.size() - 1].pos);
	}
	

}

vec2 spiderThreeApp::linecp(vec2 start, vec2 end, vec2 point) {
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

CINDER_APP( spiderThreeApp, RendererGl )
