#pragma once
#include "HelperFunctions.h"



void setInitialWeb(Graph* g, float rc) {
	auto a = boost::add_vertex(*g);
	position[a] = vec3(-5, -1, 10);
	fixedBool[a] = true;
	auto b = boost::add_vertex(*g);
	position[b] = vec3(15, 10, 0);
	fixedBool[b] = true;

	auto c = boost::add_vertex(*g);
	position[c] = vec3(10, -10, 20);
	fixedBool[c] = true;

	auto d = boost::add_vertex(*g);
	position[d] = vec3(0, 0, 0);
	fixedBool[d] = false;

	auto e = boost::add_vertex(*g);
	position[e] = vec3(-30, -40, 0);
	fixedBool[e] = true;


	connectAB(g, a, d, rc);

	cache_edge = connectAB(g, a, b, rc, 0, true);
	cache_edge = connectAB(g, a, c, rc, 0, true);
	cache_edge = connectAB(g, a, e, rc, 0, true);

	connectAB(g, b, d, rc);
	cache_edge = connectAB(g, b, c, rc, 0, true);
	cache_edge = connectAB(g, b, e, rc, 0, true);

	connectAB(g, c, d, rc);
	cache_edge = connectAB(g, c, e, rc, 0, true);


	connectAB(g, e, d, rc);

	cycles.push_back({ a,b,d });
	cycles.push_back({ a,c,d });
	cycles.push_back({ a,e,d });
	cycles.push_back({ b,c,d });
	cycles.push_back({ b,e,d });
	cycles.push_back({ c,e,d });
	addCyclesToVertices(g, cycles);
}


void setInitialWeb2(Graph* g, float rc) {
	auto a = boost::add_vertex(*g);
	position[a] = vec3(7, 8, 16);
	fixedBool[a] = true;
	auto b = boost::add_vertex(*g);
	position[b] = vec3(4, -8, 26);
	fixedBool[b] = true;

	auto c = boost::add_vertex(*g);
	position[c] = vec3(-7, -3.3, 16);
	fixedBool[c] = true;

	auto d = boost::add_vertex(*g);
	position[d] = vec3(0, 0, 0);
	fixedBool[d] = false;

	auto e = boost::add_vertex(*g);
	position[e] = vec3(0, 11, -12);
	fixedBool[e] = true;

	auto f = boost::add_vertex(*g);
	position[f] = vec3(-11, -5, -5);
	fixedBool[f] = true;

	auto h = boost::add_vertex(*g);
	position[h] = vec3(4, -14, -6);
	fixedBool[h] = true;


	connectAB(g, a, d, rc);
	connectAB(g, b, d, rc);
	connectAB(g, c, d, rc);
	connectAB(g, e, d, rc);
	connectAB(g, f, d, rc);
	connectAB(g, h, d, rc);
	
	connectAB(g, a, b, rc, 0, true);
	connectAB(g, a, c, rc, 0, true);
	connectAB(g, b, c, rc, 0, true);

	connectAB(g, a, e, rc, 0, true);
	connectAB(g, c, f, rc, 0, true);


	connectAB(g, b, h, rc, 0, true);
	
	connectAB(g, f, e, rc, 0, true);
	connectAB(g, h, e, rc, 0, true);
	connectAB(g, f, h, rc, 0, true);




	cycles.push_back({ a,b,d });
	cycles.push_back({ a,c,d });
	cycles.push_back({ b,c,d });

	cycles.push_back({ a,b,c });
	
	cycles.push_back({ a,b,h,e });
	cycles.push_back({ b,c,f,h });
	cycles.push_back({ c,a,e,f});
	cycles.push_back({ e,f,h });

	cycles.push_back({ e,f,d });
	cycles.push_back({ d,f,h });
	cycles.push_back({ e,h,d });



	addCyclesToVertices(g, cycles);
}