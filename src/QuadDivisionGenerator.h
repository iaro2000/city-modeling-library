#ifndef QUADDIVISIONGENERATOR_H
#define QUADDIVISIONGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"

#include "sstream"
#include "string"

namespace cml
{

class QuadDivisionGenerator : public IGenerator
{
public:
	QuadDivisionGenerator();
	~QuadDivisionGenerator();

	void setTarget(CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	void divide(int iterations, float minx, float miny, float maxx, float maxy);

	float minPercentLength, maxPercentLength;
	float minPercentWidth, maxPercentWidth;
	float minArea;
	int iterations;
	BBox bbox;
	float area;
	std::vector<cml::Vector2f> lines;
};

}

#endif