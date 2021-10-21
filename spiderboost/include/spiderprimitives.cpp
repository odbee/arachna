#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include "spiderprimitives.h"

vec2 linecp(vec2 start, vec2 end, vec2 point) {
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

int nClosestEdge(vec2* mousepos, Graph gr) {
	float curdist = 100000000000000.0f;
	vec2 closestp;
	int counter = 0;
	int referencer;
	for (ep = edges(g); ep.first != ep.second; ++ep.first)
	{
		vec2 p1 = g.m_vertices[source(*ep.first, g)].m_property.pos;
		vec2 p2= g.m_vertices[target(*ep.first, g)].m_property.pos;
		vec2 distp = linecp(p1, p2, *mousepos);
		float dist = distance(distp, *mousepos);
		if (dist < curdist)
		{
			curdist = dist;
			closestp = distp;
			referencer = counter;
		}
		counter++;

	}
	*mousepos = closestp;
	return referencer;

}