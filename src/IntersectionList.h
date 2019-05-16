#ifndef INTERSECTIONLIST_H
#define INTERSECTIONLIST_H

#include "Vector2.h"

namespace cml
{

//used for storing the intersection data
//for a line intersecting the edgeGraph
class IntersectionList
{
public:
	bool p1Outside;
	bool p2Outside;

	std::vector<int>	  intersectionIndexArray;
	std::vector<Vector2f> intersectionPositionArray;
};

}

#endif