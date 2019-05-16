#ifndef GENERATORMANAGER_H
#define GENERATORMANAGER_H

#include "IGenerator.h"
#include "ConvexHullGenerator.h"
#include "CircularRayGenerator.h"
#include "GridGenerator.h"
#include "QuadDivisionGenerator.h"
#include "GeometryGenerator.h"
#include "ParkGenerator.h"

namespace cml
{

class GeneratorManager
{
public:
	static GeneratorManager* instance();
	~GeneratorManager();	
	IGenerator* getGenerator(std::string name);

private:
	GeneratorManager();
	static GeneratorManager* instancePointer;

	IGenerator* convexHullGenerator;
	IGenerator* circularRayGenerator;
	IGenerator* gridGenerator;
	IGenerator* quadDivisionGenerator;
	IGenerator* geometryGenerator;
	IGenerator* parkGenerator;
};

}

#endif