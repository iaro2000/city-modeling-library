#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include "IGenerator.h"
#include "CityCell.h"
#include "Edge.h"
#include "Vector2.h"

#include "sstream"
#include "string"

namespace cml
{

class GeometryGenerator : public IGenerator
{
public:
	GeometryGenerator();
	~GeometryGenerator();

	void setTarget(CityCell *c);
	void setParameters(std::string p);
	void generate();
	void reset();

private:
	void constructSidewalkGeometry();
	bool lineInPolygon(cml::Edge line);
	cml::Vector2f getUV(cml::Vector2f position);
	std::string getRandomPrefabName();
	void triangulate(std::string materialName);
	void constructPrefabs();

	//general
	BBox bbox;
	std::vector<cml::Edge> edgeGraph;
	int geometryType;
	//relating to sidewalk
	std::string sidewalkMaterial;
	float sidewalkWidth,sidewalkHeight,sidewalkTexRepeat;
	//relating to cell floor
	std::string cellGeometryMaterial;
	float cellTexCoordRepeatX,cellTexCoordRepeatY;
	//relating to prefabs
	std::vector<cml::Vector3f> prefabPositionArray;
	std::vector<std::string> prefabNames;
	float scale,randVal;
	float prefabIntersectionDistance;
};

}

#endif