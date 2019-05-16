#ifndef CITYCELL_H
#define CITYCELL_H

//WM4 dependencies
//TODO: remove
#include "Wm4Vector2.h"
#include "Wm4PlanarGraph.h"

#include "Vector2.h"
#include "Edge.h"
#include "BBox.h"
#include "SubCell.h"
#include "Triangle.h"
#include "Prefab.h"

namespace cml
{

class CityCell
{
public:
	CityCell();
	~CityCell();

	void generate();
	void generateDebugData();
	const std::vector<cml::Edge>& getEdgeGraphData() const {return edgeGraph;};
	const std::vector<cml::Edge>& getLineGraphData() const {return lineGraph;};
	const std::vector<cml::Edge>& getDebugData() const {return debugData;};	
	const std::vector<cml::Subcell>& getSubcellData() const {return subcells;};
	const std::vector<cml::Triangle3f>& getRoadGeometryData() const {return roadGeometry;};
	const std::vector<cml::Triangle3f>& getCellGeometryData() const {return cellGeometry;};
	const std::vector<std::string>& getCellGeometryMaterialList() const {return cellGeometryMaterialList;};
	const std::vector<cml::Prefab>& getPrefabData() const {return prefabList;};
	const cml::BBox&		    getBoundingBox() const { return bbox; };
	void setBoundingBoxVisible(bool v) {showBbox = v;};
	void setBoundingBox(BBox& b) {bbox = b;};
	bool isBoundingBoxVisible() const {return showBbox;};

	int lineIntersect(cml::Vector2f p1,cml::Vector2f p2,cml::Vector2f p3, cml::Vector2f p4,cml::Vector2f &result);	
	int horizontalLineIntersect(cml::Vector2f p1,cml::Vector2f p2,cml::Vector2f p3, cml::Vector2f p4,cml::Vector2f &result);
	cml::Vector2f getPointOnLine(cml::Vector2f p1, cml::Vector2f p2,float offset);
	float   getPointOffset(cml::Vector2f p1, cml::Vector2f p2,cml::Vector2f point);
	//merge lineGraph with edgeGraph
	void lineGraphMerge();
	//takes one line segment as input
	//finds number of intersections with edgeGraph
	//either discards line or merges it with the edgeGraph
	void polygonIntersect(cml::Edge const &line);
	//merge a new line into the edgeGraph.
	//will disable the previous intersected edges, and split them in two
	//can handle merging with one or two intersection points
	void lineMerge(cml::Vector2f intersectPos1,cml::Vector2f intersectPos2,
					int edgeIndex1,int edgeIndex2 = -1,
					int intersectType1 = -1,int intersectType2 = -1);
	//for sorting intersection array generated during the edgeGraph / lineGraph
	//merging process
	void sortPointsOnLine(cml::Vector2f p1, cml::Vector2f p2,std::vector<cml::Vector2f>& positionArray,
													std::vector<int>& indexArray);
	void calculateSubcells();
	//scale (shrink) the entire subcell by a certain value
	//will move all edges inward. Distance between original and moved edges == offset
	void scaleSubcells(float offset);
	void constructRoadGeometry(float offset,float texRepeatsPerUnit);
	void addEdge(cml::Edge e, bool updateBBox = true);
	void addLine(cml::Edge e);
	void calculateAreaAndCentroid(bool onlyBoundary = false);
	float getArea() {return area;};
	cml::Vector2f getCentroid() {return centroid;};
	bool pointInPolygon(cml::Vector2f p);
	void addTriangle(cml::Triangle3f t) {cellGeometry.push_back(t);};
	void addCellGeometryMaterial(std::string m) {cellGeometryMaterialList.push_back(m);};
	void addPrefab(cml::Prefab p) {prefabList.push_back(p);};

private:
	//Root	*mRoot;
	void init();

	cml::BBox bbox;
	std::vector <cml::Edge> edgeGraph;
	std::vector <cml::Edge> lineGraph;	
	std::vector <cml::Edge> debugData;
	std::vector <cml::Triangle3f > roadGeometry;
	std::vector <cml::Triangle3f > cellGeometry;
	std::vector <std::string> cellGeometryMaterialList;
	std::vector <cml::Prefab> prefabList;
	float area;
	cml::Vector2f centroid;
	bool showBbox;

	//pentru calcul subcells (folosing minimal cycle basis)
	typedef Wm4::Vector2<float> FVector;
	typedef Wm4::Vector2<int> IVector;
	Wm4::PlanarGraph<FVector> graph;
	std::vector< Wm4::PlanarGraph<FVector>::Primitive*> primitives;
	std::vector<FVector> vertices;
	std::vector<IVector> edges;

	std::vector<cml::Subcell> subcells;
	
};

}

#endif
