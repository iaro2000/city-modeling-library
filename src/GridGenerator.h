#ifndef GRIDGENERATOR_H
#define GRIDGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"

#include "sstream"
#include "string"
#include "stdlib.h"

namespace cml
{

class GridGenerator : public IGenerator
{
public:
	GridGenerator();
	~GridGenerator();

	void setTarget(CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	float minPercentLength, maxPercentLength;
	float minPercentWidth, maxPercentWidth;
	float minAngle,maxAngle;
	BBox bbox;
};

}

#endif