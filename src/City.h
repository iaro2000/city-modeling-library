#ifndef CITY_H
#define CITY_H

#include "Edge.h"
#include "CityCell.h"
#include "BBox.h"
#include "GeneratorManager.h"
#include "IGenerator.h"
#include "ConvexHullGenerator.h"
#include "CircularRayGenerator.h"
#include "GridGenerator.h"
#include "QuadDivisionGenerator.h"
#include "GeometryGenerator.h"
#include "ParkGenerator.h"
#include "CityDefinitionFile.h"

namespace cml
{

class City
{
public:
	City();
	~City();

	void generate();
	cml::CityCell* getCell(int level,int cellNr){return cityCells[level][cellNr];};
	int getNumberOfCellsForLevel(int level);
	void reset();
	void parseCityDefinitionFile(std::string path);
	
private:	
	bool validCondition(std::string condition,int area, int randVal);

	std::vector < std::vector<cml::CityCell *> > cityCells;	
	CityDefinitionFile cdf;

};

}
#endif
