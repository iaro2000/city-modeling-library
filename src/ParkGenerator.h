#ifndef PARKGENERATOR_H
#define PARKGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"
#include "Util.h"

#include "sstream"
#include "string"

namespace cml
{

class ParkGenerator : public IGenerator
{
public:
	ParkGenerator();
	~ParkGenerator();

	void setTarget(CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	void constructCellGeometry();
	bool lineInPolygon(cml::Edge line);
	cml::Vector2f getUV(cml::Vector2f position);
	std::string getRandomPrefabName();
	void triangulate(std::string materialName);
	void constructPrefabs();
	bool pointInPolygon(cml::Vector2f p);

	//general
	BBox bbox;
	std::vector<cml::Edge> edgeGraph;
	//relating to sidewalk
	std::string sidewalkMaterial,fenceMaterial,gateMaterial;
	float sidewalkWidth,sidewalkHeight,sidewalkTexRepeat;
	float fenceHeight, fenceTexRepeat, gateHeight, gateWidth,gateAppearenceMultiplier;
	//relating to cell floor
	std::string cellGeometryMaterial;
	float cellTexCoordRepeatX,cellTexCoordRepeatY;
	//relating to prefabs
	std::vector<std::string> prefabNames;
	float minScale,maxScale;
	int minNumberOfTrees,maxNumberOfTrees;
};

}

#endif