#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "iostream"
#include "Vector3.h"
#include "Vector2.h"

namespace cml
{

//triangle
template <class T, class T2>
class templateTriangle
{
public:
	templateTriangle()
	{
	
	}

	templateTriangle(T aval, T bval, T cval, T2 atcoordval, T2 btcoordval, T2 ctcoordval)
	{
		a = aval;
		b = bval;
		c = cval;
		atcoord = atcoordval;
		btcoord = btcoordval;
		ctcoord = ctcoordval;
	}

	T a,b,c;
	T2 atcoord,btcoord,ctcoord;	
	std::string material;
};

typedef templateTriangle< Vector3f, Vector2f > Triangle3f;

}

#endif