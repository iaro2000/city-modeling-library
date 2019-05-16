#ifndef CONVEXHULLGENERATOR_H
#define CONVEXHULLGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"

#include "sstream"
#include "string"

namespace cml
{

class ConvexHullGenerator : public IGenerator
{
public:
	ConvexHullGenerator();
	~ConvexHullGenerator();

	void setTarget(cml::CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	int isLeft(cml::Vector2f p0, cml::Vector2f p1, cml::Vector2f p2);
	BBox bbox;
	int pointNr;	
};

}

#endif