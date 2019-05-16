#include "ParkGenerator.h"

cml::ParkGenerator::ParkGenerator()
{

}

cml::ParkGenerator::~ParkGenerator()
{

}

void cml::ParkGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::ParkGenerator::setParameters(std::string p)
{
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss<<p;

	bbox = target->getBoundingBox();
	
	ss>>sidewalkWidth;
	ss>>sidewalkTexRepeat;
	ss>>sidewalkHeight;

	//scale of the prefab (note: remember as cube around prefab)
	ss>>minScale>>maxScale;

	ss>>minNumberOfTrees>>maxNumberOfTrees;

	ss>>fenceHeight;
	ss>>fenceTexRepeat;
	ss>>gateHeight;
	ss>>gateWidth;
	ss>>gateAppearenceMultiplier;

	ss>>sidewalkMaterial;
	ss>>fenceMaterial;	
	ss>>gateMaterial;
	//cell material
	ss>>cellGeometryMaterial;
	//how many times to repead the cell material
	ss>>cellTexCoordRepeatX;
	ss>>cellTexCoordRepeatY;
	//parse prefabs
	std::string temp;
	//"[" string
	ss>>temp;
	//the first prefab name
	ss>>temp;
	//the prefab list
	do
	{
		prefabNames.push_back(temp.c_str());
		ss>>temp;					
	}
	while (temp != "]");		
}

void cml::ParkGenerator::generate()
{
	const std::vector<cml::Edge>& originalEdgeGraph = target->getEdgeGraphData();
	//create a copy of the original edgeGraph
	//easier to manipulate
	for (int i=0;i<originalEdgeGraph.size();i++)
	{
		cml::Edge e;
		e = originalEdgeGraph[i];
		e.partOfEdgeGraph = true;
		edgeGraph.push_back(e);
	}

	//construct the sidewalk
	target->addCellGeometryMaterial(sidewalkMaterial);
	target->addCellGeometryMaterial(fenceMaterial);
	target->addCellGeometryMaterial(gateMaterial);
	constructCellGeometry();

	//make a copy of the edgeGraph before triangulation
	//this is needed because triangulation will modify the edgeGraph
	std::vector<cml::Edge> temporaryEdgeGraph;
	for (int i=0;i<edgeGraph.size();i++)
	{
		temporaryEdgeGraph.push_back(edgeGraph[i]);
	}

	//material for the rest of the cell surface
	//(on which buildings will reside) 
	target->addCellGeometryMaterial(cellGeometryMaterial);
	triangulate(cellGeometryMaterial);

	//restore the edgeGraph to the state it was before triangulation
	edgeGraph.clear();
	for (int i=0;i<temporaryEdgeGraph.size();i++)
	{
		edgeGraph.push_back(temporaryEdgeGraph[i]);
	}
	//we no longer need the temporary edgeGraph
	temporaryEdgeGraph.clear();

	//add prefabs if needed
	constructPrefabs();

	//no longer needed
	//this edgeGraph was a working copy from cityCell anyway
	edgeGraph.clear();
}

void cml::ParkGenerator::constructPrefabs()
{
	int nrOfTrees;
	int currentNrOfTrees;
	nrOfTrees = minNumberOfTrees + rand()%maxNumberOfTrees;
	currentNrOfTrees = 0;

	while (currentNrOfTrees < nrOfTrees)
	{
		cml::Prefab p;

		//random rotation for each tree prefab
		float rotX,rotY;
		rotX = -1 + rand()%2000/(float)1000;
		rotY = -1 + rand()%2000/(float)1000;
		p.rotation = cml::Vector3f(rotX,0,rotY);

		//randomized scale of the prefab
		float scale;
		scale = minScale + (float)(rand()%(int)(maxScale*1000))/1000;
		p.scale = cml::Vector3f(scale,scale,scale);

		//randomized prefab name
		p.resourceName = prefabNames[rand()%prefabNames.size()];

		float posX,posY;
		posX = bbox.min.x + rand()%(int)(bbox.getLength());
		posY = bbox.min.y + rand()%(int)(bbox.getWidth());
		p.position = cml::Vector3f(posX,sidewalkHeight,posY);

		if (pointInPolygon(cml::Vector2f(posX,posY)))
		{
			target->addPrefab(p);
			currentNrOfTrees++;
		}
	}
}

//de mutat in clasa de utilitati
bool cml::ParkGenerator::pointInPolygon(cml::Vector2f p)
{
	//de aici:
	//http://ozviz.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
	//de cautat ref bio Philippe Reverdy SAU PiP angle, winding number, etc.

	double pi = 3.14159;
	float angle;
	cml::Edge e,next;

	angle = 0;
	for (int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e;
		cml::Vector2f p1,p2;
		e = edgeGraph[i];
		if (e.enabled && e.partOfEdgeGraph)
		{			
			p1.x = edgeGraph[i].a.x - p.x;
			p1.y = edgeGraph[i].a.y - p.y;
			
			p2.x = edgeGraph[i].b.x - p.x;
			p2.y = edgeGraph[i].b.y - p.y;

			double dtheta,theta1,theta2;

			theta1 = atan2(p1.y,p1.x);
			theta2 = atan2(p2.y,p2.x);
			dtheta = theta2-theta1;

			while (dtheta > pi)
				dtheta -= pi*2;
			while (dtheta < -pi)
				dtheta += pi*2;

			angle += dtheta;
		}
	}
		
	if (abs(angle) < pi)
		return false;
	else
		return true;
}

void cml::ParkGenerator::triangulate(std::string materialName)
{
	//triangulate
	bool enabledEdgeExists;
	enabledEdgeExists = true;
	while (enabledEdgeExists)
	{
		//find the first enabled edge in the edgeGraph
		int edgeIndex;
		enabledEdgeExists = false;
		for (int i=0;i<edgeGraph.size();i++)
		{
			if (edgeGraph[i].enabled)
			{
				enabledEdgeExists = true;
				edgeIndex = i;
				break;
			}
		}

		if (enabledEdgeExists)
		{
			cml::Edge newLine;

			//only a triangle remains			
			if (edgeGraph[edgeIndex].anb == edgeGraph[edgeGraph[edgeIndex].bnb].bnb)
			{	
				//the new edge
				newLine.a = edgeGraph[edgeIndex].a;
				newLine.b = edgeGraph[edgeGraph[edgeIndex].bnb].b;
				newLine.partOfEdgeGraph = false;

				//disable the two remaining enabled edges
				edgeGraph[edgeIndex].enabled = false;
				edgeGraph[edgeGraph[edgeIndex].bnb].enabled = false;
				edgeGraph[edgeGraph[edgeIndex].anb].enabled = false;

				//add a triangle e1->e2->newLine->back to e1
				cml::Triangle3f t;
				t.a = cml::Vector3f(
					edgeGraph[edgeIndex].b.x,
					sidewalkHeight,
					edgeGraph[edgeIndex].b.y);
				t.b = cml::Vector3f(
					edgeGraph[edgeIndex].a.x,
					sidewalkHeight,
					edgeGraph[edgeIndex].a.y);
				t.c = cml::Vector3f(
					edgeGraph[edgeGraph[edgeIndex].bnb].b.x,
					sidewalkHeight,
					edgeGraph[edgeGraph[edgeIndex].bnb].b.y);

				t.material = materialName;
				t.atcoord = getUV(edgeGraph[edgeIndex].b);
				t.btcoord = getUV(edgeGraph[edgeIndex].a);
				t.ctcoord = getUV(edgeGraph[edgeGraph[edgeIndex].bnb].b);

				target->addTriangle(t);

				//de debug
				//target->addLine(edgeGraph[edgeIndex]);
				//target->addLine(edgeGraph[edgeGraph[edgeIndex].bnb]);
				//target->addLine(newLine);
			}
			else
			{
				//add the new edge to the edgeGraph
				//used: point a of the first edge of the ear
				//point b of the neighbour edge or the first edge
				//(so the second edge of the ear)
				newLine.a = edgeGraph[edgeIndex].a;
				newLine.anb = edgeGraph[edgeIndex].anb;
				newLine.b = edgeGraph[edgeGraph[edgeIndex].bnb].b;
				newLine.bnb = edgeGraph[edgeGraph[edgeIndex].bnb].bnb;
				//thiw edge is not part of the edgeGraph
				newLine.partOfEdgeGraph = false;
				
				//if the new line is completely withing the polygon
				//clip the ear, and add the new line to the edgeGraph
				if (lineInPolygon(newLine))
				{
					edgeGraph.push_back(newLine);
					//clip the ear
					edgeGraph[edgeIndex].enabled = false;
					edgeGraph[edgeGraph[edgeIndex].bnb].enabled = false;
					edgeGraph[edgeGraph[edgeIndex].anb].bnb = edgeGraph.size()-1;
					edgeGraph[edgeGraph[edgeGraph[edgeIndex].bnb].bnb].anb = edgeGraph.size()-1;

					//de debug
					//target->addLine(edgeGraph[edgeIndex]);
					//target->addLine(edgeGraph[edgeGraph[edgeIndex].bnb]);
					//target->addLine(newLine);

					//add a triangle e1->e2->newLine->back to e1
					cml::Triangle3f t;
					t.a = cml::Vector3f(
						edgeGraph[edgeIndex].b.x,
						sidewalkHeight,
						edgeGraph[edgeIndex].b.y);
					t.b = cml::Vector3f(
						edgeGraph[edgeIndex].a.x,
						sidewalkHeight,
						edgeGraph[edgeIndex].a.y);
					t.c = cml::Vector3f(
						edgeGraph[edgeGraph[edgeIndex].bnb].b.x,
						sidewalkHeight,
						edgeGraph[edgeGraph[edgeIndex].bnb].b.y);

					t.material = materialName;
					t.atcoord = getUV(edgeGraph[edgeIndex].b);
					t.btcoord = getUV(edgeGraph[edgeIndex].a);
					t.ctcoord = getUV(edgeGraph[edgeGraph[edgeIndex].bnb].b);

					target->addTriangle(t);
				}
				else
				{					
					edgeGraph[edgeIndex].enabled = false;
				}
			}
		}
	}

}

cml::Vector2f cml::ParkGenerator::getUV(cml::Vector2f position)
{
	cml::Vector2f res;

	float bboxLength,bboxWidth;
	float length,width;

	bboxLength = abs(bbox.max.x - bbox.min.x);
	bboxWidth  = abs(bbox.max.y - bbox.min.y);

	length = abs(position.x - bbox.min.x);
	width  = abs(position.y - bbox.min.y);

	res.x = (length * cellTexCoordRepeatX) / bboxLength;
	res.y = (width  * cellTexCoordRepeatY) / bboxWidth;

	return res;
}

std::string cml::ParkGenerator::getRandomPrefabName()
{
	return prefabNames[rand()%prefabNames.size()];
}

bool cml::ParkGenerator::lineInPolygon(cml::Edge line)
{
	for (int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e;
		cml::Vector2f result;

		e = edgeGraph[i];

		if (e.enabled)
		{
			//an intersection occured. The line exited the polygon
			//return false => not completely in polygon
			if (target->lineIntersect(e.a,e.b,line.a,line.b,result) == 1)
			{
				return false;
			}			
		}
	}
	return true;
}

void cml::ParkGenerator::constructCellGeometry()
{
	std::vector<cml::Edge> scaledEdgeGraph;

	for (int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e,eaneighbour,ebneighbour;
		cml::Vector2f normal,middle;

		e = edgeGraph[i];		
		eaneighbour	= edgeGraph[e.anb];
		ebneighbour	= edgeGraph[e.bnb];
		
		//if (!e.partOfEdgeGraph)
		{
			normal = e.b - e.a;
			//pentru cazul curent va rezulta o perpendiculara interioara subcelulei
			normal = normal.perpendicular();	
			//pentru perpendiculara exterioara
			//normal *= -1;
			//normalizeaza normala (perpendiculara)
			// => lungime standard 1, care poate fi scalata
			normal = normal.normalize();
			//scalarea distantei cu o anumita valoare
			//?? poate dependenta de suprafata / dimensiunia bbox-ului subcelulei ??
			normal *= sidewalkWidth;
			//pentru punctul de mijloc
			//middle = e.a + e.b;
			//middle = middle.midpoint();	
			//pentru normala la mijlocul segmentului
			//lineGraph.push_back(cml::Edge(middle,middle+normal));

			e.a = e.a + normal;
			e.b = e.b + normal;
		}

		//if (!eaneighbour.partOfEdgeGraph)
		{
			normal = eaneighbour.b - eaneighbour.a;
			normal = normal.perpendicular();
			normal = normal.normalize();
			normal *= sidewalkWidth;
			eaneighbour.a = eaneighbour.a + normal;
			eaneighbour.b = eaneighbour.b + normal;
		}

		//if (!ebneighbour.partOfEdgeGraph)
		{
			normal = ebneighbour.b - ebneighbour.a;
			normal = normal.perpendicular();
			normal = normal.normalize();
			normal *= sidewalkWidth;
			ebneighbour.a = ebneighbour.a + normal;
			ebneighbour.b = ebneighbour.b + normal;
		}

		cml::Vector2f p1,p2;

		target->lineIntersect(e.a,e.b,eaneighbour.a,eaneighbour.b,p1);
		target->lineIntersect(e.a,e.b,ebneighbour.a,ebneighbour.b,p2);

		e.a = p1;
		e.b = p2;

		normal = p1 - p2;
		normal = normal.perpendicular();
		normal = normal.normalize();
		normal *= -sidewalkWidth;
		p1 = p1 - normal;
		p2 = p2 - normal;			

		float divisionPoint1,divisionPoint2;
		divisionPoint1 = target->getPointOffset(edgeGraph[i].a,edgeGraph[i].b,p1);			
		divisionPoint2 = target->getPointOffset(edgeGraph[i].a,edgeGraph[i].b,p2);

		float total;
		float edgeLength;

		edgeLength = (edgeGraph[i].a - edgeGraph[i].b).length();
		total = edgeLength*sidewalkTexRepeat;
		total = (int)total+0.004;
		if (total < 1) total = 1.004;
		
		//if (!edgeGraph[i].partOfEdgeGraph)
		{
			cml::Triangle3f t;

			float coord1,coord2;
			
			coord1 = total - divisionPoint1*total;
			coord2 = total - divisionPoint2*total;

			//sidewalk triangle 1
			t.a = cml::Vector3f(edgeGraph[i].b.x,sidewalkHeight,edgeGraph[i].b.y);
			t.b = cml::Vector3f(edgeGraph[i].a.x,sidewalkHeight,edgeGraph[i].a.y);			
			t.c = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);
			t.atcoord = cml::Vector2f(0,0);
			t.btcoord = cml::Vector2f(0,total);
			t.ctcoord = cml::Vector2f(1,coord1);
			t.material = sidewalkMaterial;
			target->addTriangle(t);
			//sidewalk triangle 2
			t.a = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);
			t.b = cml::Vector3f(e.b.x,sidewalkHeight,e.b.y);
			t.c = cml::Vector3f(edgeGraph[i].b.x,sidewalkHeight,edgeGraph[i].b.y);
			t.atcoord = cml::Vector2f(1,coord1);
			t.btcoord = cml::Vector2f(1,coord2);
			t.ctcoord = cml::Vector2f(0,0);
			t.material = sidewalkMaterial;
			target->addTriangle(t);
			//sidewalk edge triangle 1
			t.a = cml::Vector3f(edgeGraph[i].b.x,0,edgeGraph[i].b.y);
			t.b = cml::Vector3f(edgeGraph[i].a.x,0,edgeGraph[i].a.y);
			t.c = cml::Vector3f(edgeGraph[i].a.x,sidewalkHeight,edgeGraph[i].a.y);
			t.atcoord = cml::Vector2f(0,0);
			t.btcoord = cml::Vector2f(0,total);
			t.ctcoord = cml::Vector2f(sidewalkHeight/sidewalkWidth,total);
			t.material = sidewalkMaterial;
			target->addTriangle(t);
			//sidewalk edge triangle 2
			t.a = cml::Vector3f(edgeGraph[i].a.x,sidewalkHeight,edgeGraph[i].a.y);				
			t.b = cml::Vector3f(edgeGraph[i].b.x,sidewalkHeight,edgeGraph[i].b.y);
			t.c = cml::Vector3f(edgeGraph[i].b.x,0,edgeGraph[i].b.y);
			t.atcoord = cml::Vector2f(sidewalkHeight/sidewalkWidth,total);
			t.btcoord = cml::Vector2f(sidewalkHeight/sidewalkWidth,0);
			t.ctcoord = cml::Vector2f(0,0);
			t.material = sidewalkMaterial;
			target->addTriangle(t);

			//gate width -> offset of edge calculation
			float gateWidthInOffset;
			cml::Vector2f gatePoint1,gatePoint2,middle;

			//if length gate is at least gateAppearenceMultiplier times larger than edge length
			if (gateWidth*gateAppearenceMultiplier < edgeLength)
			{
				gateWidthInOffset = gateWidth/edgeLength;
				//gate is made up of two segments
				gateWidthInOffset/= 2;				

				gatePoint1 = target->getPointOnLine(e.a,e.b,0.5-gateWidthInOffset);
				gatePoint2 = target->getPointOnLine(e.a,e.b,0.5+gateWidthInOffset);
				middle = target->getPointOnLine(e.a,e.b,0.5);

				//fence triangle 1
				t.a = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);				
				t.b = cml::Vector3f(gatePoint1.x,sidewalkHeight,gatePoint1.y);
				t.c = cml::Vector3f(gatePoint1.x,sidewalkHeight+fenceHeight,gatePoint1.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,1);
				t.ctcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,0);
				t.material = fenceMaterial;
				target->addTriangle(t);

				//fence triangle 2
				t.a = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);				
				t.b = cml::Vector3f(gatePoint1.x,sidewalkHeight+fenceHeight,gatePoint1.y);
				t.c = cml::Vector3f(e.a.x,sidewalkHeight+fenceHeight,e.a.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,0);
				t.ctcoord = cml::Vector2f(0,0);
				t.material = fenceMaterial;
				target->addTriangle(t);

				//fence triangle 3
				t.a = cml::Vector3f(gatePoint2.x,sidewalkHeight,gatePoint2.y);				
				t.b = cml::Vector3f(e.b.x,sidewalkHeight,e.b.y);
				t.c = cml::Vector3f(e.b.x,sidewalkHeight+fenceHeight,e.b.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,1);
				t.ctcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,0);
				t.material = fenceMaterial;
				target->addTriangle(t);

				//fence triangle 4
				t.a = cml::Vector3f(gatePoint2.x,sidewalkHeight,gatePoint2.y);				
				t.b = cml::Vector3f(e.b.x,sidewalkHeight+fenceHeight,e.b.y);
				t.c = cml::Vector3f(gatePoint2.x,sidewalkHeight+fenceHeight,gatePoint2.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f((edgeLength/2 - gateWidth)*fenceTexRepeat,0);
				t.ctcoord = cml::Vector2f(0,0);
				t.material = fenceMaterial;
				target->addTriangle(t);

				//gate triangle 1
				t.a = cml::Vector3f(gatePoint1.x,sidewalkHeight,gatePoint1.y);				
				t.b = cml::Vector3f(middle.x,sidewalkHeight,middle.y);
				t.c = cml::Vector3f(middle.x,sidewalkHeight+gateHeight,middle.y);
				t.atcoord = cml::Vector2f(0.01,0.99);
				t.btcoord = cml::Vector2f(0.99,0.99);
				t.ctcoord = cml::Vector2f(0.99,0.01);
				t.material = gateMaterial;
				target->addTriangle(t);

				//gate triangle 2
				t.a = cml::Vector3f(gatePoint1.x,sidewalkHeight,gatePoint1.y);				
				t.b = cml::Vector3f(middle.x,sidewalkHeight+gateHeight,middle.y);
				t.c = cml::Vector3f(gatePoint1.x,sidewalkHeight+gateHeight,gatePoint1.y);
				t.atcoord = cml::Vector2f(0.01,0.99);
				t.btcoord = cml::Vector2f(0.99,0.01);
				t.ctcoord = cml::Vector2f(0.01,0.01);
				t.material = gateMaterial;
				target->addTriangle(t);

				//gate triangle 3
				t.a = cml::Vector3f(middle.x,sidewalkHeight,middle.y);				
				t.b = cml::Vector3f(gatePoint2.x,sidewalkHeight,gatePoint2.y);
				t.c = cml::Vector3f(gatePoint2.x,sidewalkHeight+gateHeight,gatePoint2.y);
				t.atcoord = cml::Vector2f(0.99,0.99);
				t.btcoord = cml::Vector2f(0.01,0.99);
				t.ctcoord = cml::Vector2f(0.01,0.01);
				t.material = gateMaterial;
				target->addTriangle(t);

				//gate triangle 4
				t.a = cml::Vector3f(middle.x,sidewalkHeight,middle.y);				
				t.b = cml::Vector3f(gatePoint2.x,sidewalkHeight+gateHeight,gatePoint2.y);
				t.c = cml::Vector3f(middle.x,sidewalkHeight+gateHeight,middle.y);
				t.atcoord = cml::Vector2f(0.99,0.99);
				t.btcoord = cml::Vector2f(0.01,0.01);
				t.ctcoord = cml::Vector2f(0.99,0.01);
				t.material = gateMaterial;
				target->addTriangle(t);
			}
			//add a fence without a gate
			else
			{
				//fence triangle 1
				t.a = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);				
				t.b = cml::Vector3f(e.b.x,sidewalkHeight,e.b.y);
				t.c = cml::Vector3f(e.b.x,sidewalkHeight+fenceHeight,e.b.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f(edgeLength*fenceTexRepeat,1);
				t.ctcoord = cml::Vector2f(edgeLength*fenceTexRepeat,0);
				t.material = fenceMaterial;
				target->addTriangle(t);

				//fence triangle 2
				t.a = cml::Vector3f(e.a.x,sidewalkHeight,e.a.y);				
				t.b = cml::Vector3f(e.b.x,sidewalkHeight+fenceHeight,e.b.y);
				t.c = cml::Vector3f(e.a.x,sidewalkHeight+fenceHeight,e.a.y);
				t.atcoord = cml::Vector2f(0,1);
				t.btcoord = cml::Vector2f(edgeLength*fenceTexRepeat,0);
				t.ctcoord = cml::Vector2f(0,0);
				t.material = fenceMaterial;
				target->addTriangle(t);
			}
		}

		scaledEdgeGraph.push_back(e);
	}

	for (int i=0;i<edgeGraph.size();i++)
	{
		scaledEdgeGraph[i].partOfEdgeGraph = true;
		edgeGraph[i] = scaledEdgeGraph[i];
	}
	scaledEdgeGraph.clear();
}

void cml::ParkGenerator::reset()
{
	//TODO: de completat atat aici cat si pentru alte generatoare
	//chestii de genul clear() la vectorul de stringuri la prefaburi
}