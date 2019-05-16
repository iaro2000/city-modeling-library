#include "GeneratorManager.h"

cml::GeneratorManager* cml::GeneratorManager::instancePointer = 0;

cml::GeneratorManager::GeneratorManager()
{
	convexHullGenerator = NULL;
	circularRayGenerator = NULL;
	gridGenerator = NULL;
	quadDivisionGenerator = NULL;
	geometryGenerator = NULL;
	parkGenerator = NULL;
}

cml::GeneratorManager::~GeneratorManager()
{
	if (instancePointer != 0)
	{
		delete instancePointer;
	}
}

cml::GeneratorManager* cml::GeneratorManager::instance()
{
	if (instancePointer == 0)
	{
		instancePointer = new GeneratorManager();
	}
	return instancePointer;
}

cml::IGenerator* cml::GeneratorManager::getGenerator(std::string name)
{
	if (name == "ConvexHullGenerator")
	{
		if (convexHullGenerator == NULL)
		{
			convexHullGenerator = new ConvexHullGenerator();
		}	
		return convexHullGenerator;
	}

	if (name == "CircularRayGenerator")
	{
		if (circularRayGenerator == NULL)
		{
			circularRayGenerator = new CircularRayGenerator();
		}		
		return circularRayGenerator;
	}

	if (name == "GridGenerator")
	{
		if (gridGenerator == NULL)
		{
			gridGenerator = new GridGenerator();
		}		
		return gridGenerator;
	}

	if (name == "QuadDivisionGenerator")
	{
		if (quadDivisionGenerator == NULL)
		{
			quadDivisionGenerator = new QuadDivisionGenerator();
		}		
		return quadDivisionGenerator;
	}

	if (name == "GeometryGenerator")
	{
		if (geometryGenerator == NULL)
		{
			geometryGenerator = new GeometryGenerator();
		}		
		return geometryGenerator;
	}

	if (name == "ParkGenerator")
	{
		if (parkGenerator == NULL)
		{
			parkGenerator = new ParkGenerator();
		}		
		return parkGenerator;
	}
}