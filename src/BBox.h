#ifndef BBOX_H
#define BBOX_H

#include "Vector2.h"
#include "math.h"

namespace cml
{

class BBox
{
public:
	BBox();
	BBox(cml::Vector2f maximum, cml::Vector2f minimum);
	~BBox();

	cml::Vector2f min;
	cml::Vector2f max;	

	void addLine(cml::Vector2f p1, cml::Vector2f p2);
	void addPadding(float padding);
	void repair();
	float getLength() {return fabs(max.x - min.x); };
	float getWidth() {return fabs(max.y - min.y); };
};

}

#endif
