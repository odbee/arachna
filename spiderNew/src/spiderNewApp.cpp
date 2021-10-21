#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"

#include "spidergraph.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>


using namespace ci;
using namespace ci::app;
using namespace std;

#define RELAXMULT  0.9f

class spiderNewApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	//void mouseDown(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	vec3 p1 = vec3(-5, -1, 10);
	vec3 p2 = vec3(15, 10, 0);
	vec3 p3 = vec3(10, -10, 20);
	vec3 p4 = vec3(-30,-40,-30);
	CameraPersp		mCamera;




private:
	gl::BatchRef		mWirePlane;
	CameraUi		mCamUi;
	ivec2			mMousePos;
};

void spiderNewApp::setup()
{
	//			CAMERA SETUP
	{
		mCamera.lookAt(vec3(30.0f, 20.0f, 40.0f), vec3(0));
		mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 1000.0f);
		mCamUi = CameraUi(&mCamera, getWindow());
		auto lambertShader = gl::getStockShader(gl::ShaderDef().color().lambert());
		auto colorShader = gl::getStockShader(gl::ShaderDef().color());
		mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(10)).subdivisions(ivec2(10)), colorShader);
	}
	//			END CAMERA SETUP

	auto a = boost::add_vertex(g); g[a].isfixed = true; g[a].pos = p1;
	auto b = boost::add_vertex(g); g[b].isfixed = true; g[b].pos = p2;
	auto c = boost::add_vertex(g); g[c].isfixed = true; g[c].pos = p3;
	auto d = boost::add_vertex(g); g[d].isfixed = true; g[d].pos = p4;

	auto dd = distance(position[a], position[d]);
	auto edg1 = boost::add_edge(a, d, g).first;
	
	currentLengthPm[edg1] = dd;
	restLengthPm[edg1] = dd * 0.7;
	//g[edg1].restlength;
	
	edg1= boost::add_edge(b, d, g).first;
	dd = distance(position[b], position[d]);
	currentLengthPm[edg1] = dd;
	restLengthPm[edg1] = dd * 0.7;

	edg1= boost::add_edge(c, d, g).first;
	dd = distance(position[c], position[d]);
	currentLengthPm[edg1] = dd;
	restLengthPm[edg1] = dd * 0.7;


}

void spiderNewApp::keyDown(KeyEvent event) {
	console() << ":" << event.getCode() << ":" << endl;
	if (event.getCode() == 99) {
		console() << "true" << endl;
		tie(ei, eiend) = boost::edges(g);
		int iteratorLength = 0;
		for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
			iteratorLength++;
		}
		console() << iteratorLength << endl;
		auto e1 = boost::add_vertex(g); g[e1].isfixed = false;
		auto e2 = boost::add_vertex(g); g[e2].isfixed = false;

		ei = boost::edges(g).first;
		auto randiter = rand() % iteratorLength;
		for (size_t i = 0; i < randiter; i++)
		{
			ei++;
		}
		auto eii = ei;
		auto tp1 = position[source(*ei, g)];
		auto tp2 = position[target(*ei, g)];
		auto randiter2 = rand() % iteratorLength;
		g[e1].pos = tp1 + (tp2 - tp1) * float(float(rand() % 1000) / 1000);

		while (randiter == randiter2)
		{
			randiter2 = rand() % iteratorLength;
		}
		ei = boost::edges(g).first;
		for (size_t i = 0; i < randiter2; i++)
		{
			ei++;
		}
		tp1 = position[source(*ei, g)];
		tp2 = position[target(*ei, g)];
		g[e2].pos = tp1 + (tp2 - tp1) * float(float(rand() % 1000) / 1000);


		auto edgo = boost::add_edge(e1, e2, g).first;
		
		auto dd = distance(position[e1], position[e2]);


		currentLengthPm[edgo] = dd;
		restLengthPm[edgo] = dd * 0.8;
		
		auto anotheredge1 = boost::add_edge(e2, source(*ei,g) ,g).first;
		restLengthPm[anotheredge1] = distance(
			position[source(anotheredge1, g)], position[target(anotheredge1, g)])
			*0.8;
		
		currentLengthPm[anotheredge1] = distance(
			position[source(anotheredge1, g)], position[target(anotheredge1, g)])
			* 1;

		auto anotheredge2 = boost::add_edge(e2, target(*ei, g), g).first;
		restLengthPm[anotheredge2] = distance(
			position[source(anotheredge2, g)], position[target(anotheredge2, g)])
			* 0.8;

		currentLengthPm[anotheredge2] = distance(
			position[source(anotheredge2, g)], position[target(anotheredge2, g)])
			* 1;

		auto anotheredge3 = boost::add_edge(e1, source(*eii, g), g).first;
		restLengthPm[anotheredge3] = distance(
			position[source(anotheredge3, g)], position[target(anotheredge3, g)])
			* 0.8;
		currentLengthPm[anotheredge3] = distance(
			position[source(anotheredge3, g)], position[target(anotheredge3, g)])
			* 1;
		auto anotheredge4 = boost::add_edge(e1, target(*eii, g), g).first;
		restLengthPm[anotheredge4] = distance(
			position[source(anotheredge4, g)], position[target(anotheredge4, g)])
			* 0.8;
		currentLengthPm[anotheredge4] = distance(
			position[source(anotheredge4, g)], position[target(anotheredge4, g)])
			* 1;


		boost::remove_edge(*eii, g);
		boost::remove_edge(*ei, g);

		for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

			//gl::drawStrokedCube(position[*vi], vec3(0.5f, 0.5f, 0.5f));
			console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}
}


void spiderNewApp::update()
{
	if (true) {
		float k = 1.1f;
		float eps = 0.1f;
		Graph::vertex_descriptor v1, v2;
		for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
			v1 = boost::source(*ei, g);
			v2 = boost::target(*ei, g);
			if (!g[v1].isfixed) {
				moveVecPm[v1] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v2] - position[v1]);
			}
			if (!g[v2].isfixed) {
				moveVecPm[v2] += 1 * k * (currentLengthPm[*ei] - restLengthPm[*ei]) *
					normalize(position[v1] - position[v2]);
			}
		}

		for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

			//position[*vi] += moveVecPm[*vi];
			g[*vi].pos += eps* moveVecPm[*vi];
			moveVecPm[*vi] = vec3(0, 0, 0);
		}
		for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
			v1 = boost::source(*ei, g);
			v2 = boost::target(*ei, g);
			currentLengthPm[*ei] = distance(position[v1], position[v2]);
		}
	}
}

void spiderNewApp::draw()
{

	gl::clear(Color::gray(0.5f));
	gl::color(1.0f, 1.0f, 1.0f, 0.5f);

	//			CAMERA DRAW
	{
		mat4 projection = mCamera.getProjectionMatrix() * mCamera.getViewMatrix();
		int w = getWindowWidth();
		int h = getWindowHeight();
		vec4 viewport = vec4(0, h, w, -h); // vertical flip is required

		vec2 screencoord = glm::project(vec3(0, 0, 0), mat4(), projection, viewport);
		gl::drawSolidCircle(screencoord, 10);
		for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
			vec3 danchorp = position[boost::source(*ei, g)] + (position[boost::target(*ei, g)] - position[boost::source(*ei, g)]) * 0.5f;
			vec2 anchorp1 = glm::project(danchorp, mat4(), projection, viewport);
			//gl::drawString(to_string(restLengthPm[*ei]), anchorp1);
			
			//gl::drawString(to_string(currentLengthPm[*ei]), anchorp1+vec2(0,10));

		}
		gl::ScopedMatrices push;
		gl::setMatrices(mCamera);
		{
			gl::ScopedColor color(Color::gray(0.2f));
			//mWirePlane->draw();
			


			gl::color(1.0f, 1.0f, 1.0f, 0.8f);
			for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
				gl::drawLine(position[boost::source(*ei, g)], position[boost::target(*ei, g)]);
				vec3 danchorp = position[boost::source(*ei, g)]+(position[boost::target(*ei, g)]- position[boost::source(*ei, g)])*0.5f;
				vec2 anchorp1= glm::project(danchorp, mat4(), projection, viewport);
				gl::drawString(to_string(123), anchorp1);
			
			}
			gl::color(1.0f, 1.0f, 1.0f, 1.0f);
			for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

				gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
				
				//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
			}
		}
	}
	//			END CAMERA DRAW


}
CINDER_APP( spiderNewApp, RendererGl )