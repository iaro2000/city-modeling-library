#include "GeometryGenerator.h"

cml::GeometryGenerator::GeometryGenerator()
{

}

cml::GeometryGenerator::~GeometryGenerator()
{

}

void cml::GeometryGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::GeometryGenerator::setParameters(std::string p)
{
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss<<p;

	bbox = target->getBoundingBox();
	
	ss>>sidewalkWidth;
	ss>>sidewalkTexRepeat;
	ss>>sidewalkHeight;

	//scale of the prefab (note: remember as cube around prefab)
	ss>>scale;
	//random added distance between two prefabs on the same line
	ss>>randVal;
	//minimal planar distance (not on edge) (for intersection testing)
	ss>>prefabIntersectionDistance;

	//sidewalk material
	ss>>sidewalkMaterial;
	//cell material
	ss>>cellGeometryMaterial;
	//how many times to repead the cell material
	ss>>cellTexCoordRepeatX;
	ss>>cellTexCoordRepeatY;
	//type of geometry (for now only prefabs)
	ss>>geometryType;
	//if the generator must use prefabs
	if (geometryType == 0)
	{
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
}

void cml::GeometryGenerator::generate()
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
	//temporar, de inlaturat daca nu mai apar probleme cu triangularea
	//temp();

	//construct the sidewalk
	target->addCellGeometryMaterial(sidewalkMaterial);
	constructSidewalkGeometry();

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
	if (geometryType == 0) constructPrefabs();

	//no longer needed
	//this edgeGraph was a working copy from cityCell anyway
	edgeGraph.clear();
}

void cml::GeometryGenerator::constructPrefabs()
{
	//create the prefabs.
	//for each edge in the edgeGraph,
	//add a number of prefabs
	for (int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e,ea,eb;
		//the edge and its two neighbour
		e = edgeGraph[i];
		ea = edgeGraph[e.anb];
		eb = edgeGraph[e.bnb];
		//only the edges that form the boundary
		if (e.partOfEdgeGraph)
		{
			cml::Prefab p;
			cml::Vector2f direction,normal;
			float edgeLength,currentPosition,distanceFromEdgeEnd;
			//buffer value for the start/stop points on the edge
			distanceFromEdgeEnd = scale;
			
			//for prefab rotation
			normal = e.b - e.a;
			normal = normal.perpendicular();	
			normal = normal.normalize();
			//for prefab position
			direction = e.b - e.a;
			edgeLength = sqrt(direction.x*direction.x + direction.y*direction.y);
			direction = direction.normalize();
			
			//starting position on the line
			currentPosition = distanceFromEdgeEnd;

			//while more prefabs can be added
			while (currentPosition < edgeLength - distanceFromEdgeEnd)
			{	
				//general prefab parameters
				//p.resourceName = "building16.mesh";
				p.resourceName = prefabNames[rand()%prefabNames.size()];
				p.position = cml::Vector3f(e.a.x+currentPosition*direction.x,
											sidewalkHeight,
											e.a.y+currentPosition*direction.y);
				p.rotation = cml::Vector3f(normal.x,0,normal.y);
				p.scale = cml::Vector3f(scale,scale,scale);
				
				//prefab bounding box diagonals
				cml::Edge diagonal1,diagonal2;
				//intersection result; not needed
				cml::Vector2f r;

				diagonal1.a = cml::Vector2f(
					p.position.x+scale*direction.x,
					p.position.z+scale*direction.y
					);
				diagonal1.b = cml::Vector2f(
					p.position.x-scale*direction.x+scale*2*normal.x,
					p.position.z-scale*direction.y+scale*2*normal.y
					);

				diagonal2.a = cml::Vector2f(
					p.position.x-scale*direction.x,
					p.position.z-scale*direction.y
					);
				diagonal2.b = cml::Vector2f(
					p.position.x+scale*direction.x+scale*2*normal.x,
					p.position.z+scale*direction.y+scale*2*normal.y
					);

				//de debug
				//target->addLine(diagonal1);
				//target->addLine(diagonal2);

				//if the diagonals of the prefab do not intersect
				//either of the edge neighbours, we can add the prefab.
				if ((target->lineIntersect(ea.a,ea.b,diagonal1.a,diagonal1.b,r) != 1)
					&&
					(target->lineIntersect(eb.a,eb.b,diagonal1.a,diagonal1.b,r) != 1)
					&&
					(target->lineIntersect(ea.a,ea.b,diagonal2.a,diagonal2.b,r) != 1)
					&&
					(target->lineIntersect(eb.a,eb.b,diagonal2.a,diagonal2.b,r) != 1))
				{
					//also check to see a minimum (planar) distance between prefabs
					//so we can avoid unwanted geometrical intersections
					bool valid = true;
					for (int i=0;i<prefabPositionArray.size();i++)
					{
						float distance;
						distance = sqrt( (p.position.x-prefabPositionArray[i].x)
										 *
										 (p.position.x-prefabPositionArray[i].x)
										 +
										 (p.position.z-prefabPositionArray[i].z)
										 *
										 (p.position.z-prefabPositionArray[i].z) );
						if (distance < prefabIntersectionDistance)
						{
							valid = false;
							break;
						}
					}
					
					if (valid)
					{
						target->addPrefab(p);
						prefabPositionArray.push_back(p.position);
					}
				}
				
				//the position of the next prefab
				if (randVal >= 0)
				{
					currentPosition += scale*2;
					currentPosition += (float)(rand()%((int)(randVal*1000)))/1000;
				}
				else
				{
					currentPosition += scale*2;
					currentPosition -= (float)(rand()%((int)((-randVal)*1000)))/1000;
				}
			}//Finished adding prefabs
			//no longer needed
			prefabPositionArray.clear();
		}
	}
}

void cml::GeometryGenerator::triangulate(std::string materialName)
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

					t.material = "cellGeometryMaterial";
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

cml::Vector2f cml::GeometryGenerator::getUV(cml::Vector2f position)
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

std::string cml::GeometryGenerator::getRandomPrefabName()
{
	return prefabNames[rand()%prefabNames.size()];
}

bool cml::GeometryGenerator::lineInPolygon(cml::Edge line)
{
	//TODO:
	//daca avem caz de genul: /-\ cu linia de test intre colturile
	//jos ale /\ trebuie adaugat ceva check in plus,
	//pt. ca linia e in exterior.
	//Nu merge cu point in polygon pentru colturi,
	//pentru ca fac parte din polygon.
	//Poate un check pointInPolygon la midpoint?
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

void cml::GeometryGenerator::constructSidewalkGeometry()
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

void cml::GeometryGenerator::reset()
{

}