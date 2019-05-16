#ifndef CIRCULARRAYGENERATOR_H
#define CIRCULARRAYGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"

#include "sstream"
#include "string"

namespace cml
{

class CircularRayGenerator : public IGenerator
{
public:
	CircularRayGenerator();
	~CircularRayGenerator();

	void setTarget(CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	int minRays, maxRays;
	//TODO: float angles => rand()%(min/maxAngle*1000) / 1000
	//where 1000 - precision. eg: 76.455
	int minAngle,maxAngle;
	BBox bbox;
	cml::Vector2f centerPoint;
};

}

#endif