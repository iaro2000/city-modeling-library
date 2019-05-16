#include "CityCell.h"

cml::CityCell::CityCell()
{
	init();
}

cml::CityCell::~CityCell()
{
	edgeGraph.clear();
	lineGraph.clear();
	subcells.clear();
}

void cml::CityCell::init()
{	
	area = -1;
	centroid = cml::Vector2f(0,0);
	showBbox = true;
}

void cml::CityCell::generate()
{
	//Ogre::Timer t;
	//t.reset();

	//lineGraphMerge();

	/*
	//scalarea vertecsilor; poate va fi folositor
	float val;
	val = 16;
	for (int i=0;i<edgeGraph.size();i++)
	{
		edgeGraph[i].a.x *= val;
		edgeGraph[i].a.y *= val;
		edgeGraph[i].b.x *= val;
		edgeGraph[i].b.y *= val;
	}
	bbox.min.x *= val;
	bbox.min.y *= val;
	bbox.max.x *= val;
	bbox.max.y *= val;
	*/

	//calculateSubcells();
	//scaleSubcells(0.9);

	//generateDebugData();

	//std::cout<<"\nGeneration in "<<t.getMicroseconds()<<" microseconds\n";
	//calcule neighbours versiune veche ~1850 microseconds
	//	- din cauza la std::map. Cu std::vector pt. neighbour ~1200 microseconds	
}

int cml::CityCell::lineIntersect(cml::Vector2f p1,cml::Vector2f p2,cml::Vector2f p3, cml::Vector2f p4, cml::Vector2f& result)
{
	double denominator,numerator1,numerator2;

	denominator = (p4.y-p3.y)*(p2.x-p1.x) - (p4.x-p3.x)*(p2.y-p1.y);
	numerator1  = (p4.x-p3.x)*(p1.y-p3.y) - (p4.y-p3.y)*(p1.x-p3.x);
	numerator2  = (p2.x-p1.x)*(p1.y-p3.y) - (p2.y-p1.y)*(p1.x-p3.x);

	//de debug
	/*
	std::cout<<"denominator: "<<denominator<<std::endl;
	std::cout<<"numerator1: " <<numerator1 <<std::endl;
	std::cout<<"numerator2: " <<numerator2 <<std::endl;
	*/

	if (denominator == 0)
	{
		//dreptele coincid
		if (numerator1 == 0 && numerator2 == 0)
		{
			return 3;
		}

		//dreptele sunt paralele
		return 2;
	}

	float ua,ub;
	cml::Vector2f res;
	ua = numerator1/denominator;
	ub = numerator2/denominator;

	//reintroducem ua si ub intr-una din drepte
	res.x = p1.x + ua*(p2.x-p1.x);
	res.y = p1.y + ua*(p2.y-p1.y);

	//de debug
	/*
	std::cout<<"ua: " <<ua<<std::endl;
	std::cout<<"ub: " <<ub<<std::endl;
	std::cout<<"intersect.x: " <<res.x<<std::endl;
	std::cout<<"intersect.y: " <<res.y<<std::endl;
	*/

	//testam tipul de intersectie - intersectie de segment sau de linii
	//nu lua in considerare cazul extrem cu <=,>= pentru intersectii la limita segmentului
	if ((ua >0 && ua <1) && (ub >0 && ub <1))
	{
		result = res;
		//segment intersect
		return 1;
	}
	else
	{
		result = res;
		//line intersect
		return 0;
	}

}

int cml::CityCell::horizontalLineIntersect(cml::Vector2f p1,cml::Vector2f p2,cml::Vector2f p3, cml::Vector2f p4, cml::Vector2f &result)
{
	double denominator,numerator1,numerator2;

	denominator = (p3.x-p4.x)*(p2.y-p1.y);
	numerator1  = (p4.x-p3.x)*(p1.y-p3.y);
	numerator2  = (p2.x-p1.x)*(p1.y-p3.y) - (p2.y-p1.y)*(p1.x-p3.x);

	if (denominator == 0)
	{
		//dreptele coincid
		if (numerator1 == 0 && numerator2 == 0)
		{
			return 3;
		}

		//dreptele sunt paralele
		return 2;
	}

	float ua,ub;
	cml::Vector2f res;
	ua = numerator1/denominator;
	ub = numerator2/denominator;

	//reintroducem ua si ub intr-una din drepte
	res.x = p3.x + ub*(p4.x-p3.x);
	res.y = p3.y;

	//testam tipul de intersectie - intersectie de segment sau de linii
	if ((ua >=0 && ua <=1) && (ub >=0 && ub <=1))
	{
		result = res;
		//segment intersect
		return 1;
	}
	else
	{
		result = res;
		//line intersect
		return 0;
	}
}

void cml::CityCell::generateDebugData()
{
	cml::Vector2f p1,p2;
	cml::Edge e,temp;
	std::map<int,int>::iterator mapIter;

	float relativeStartOffset = 0.02;
	float relativeEndOffset	  = 0.015;
	
	//XZ (XY) axes
	e.a = cml::Vector2f(-10000,0);
	e.b = cml::Vector2f( 10000,0);
	debugData.push_back(e);
	e.a = cml::Vector2f(0,-10000);
	e.b = cml::Vector2f(0, 10000);
	debugData.push_back(e);

	for (unsigned int i=0;i<edgeGraph.size();i++)
	{
		e = edgeGraph[i];
		
		//do not add debug data for disabled edges
		if (e.enabled)
		{
			//construct debug data for all neighbours of point a of edge e
			p1 = getPointOnLine(e.a,e.b, relativeStartOffset);
			//iterate over all the neighbours of point a; meaning anb vector
			//for (mapIter = e.anb.begin();mapIter != e.anb.end();++mapIter)
			//nu mai avem nevoie de iteratie. Doar 1 nb
			//for (int i = 0;i<e.anb.size();i++)
			if (e.anb != -1)
			{
				cml::Edge neighbourEdge;
				//get the neighbour edge from the edgegraph
				//neighbourEdge = edgeGraph[mapIter->first];
				//neighbourEdge = edgeGraph[e.anb[i]];
				neighbourEdge = edgeGraph[e.anb];

				//the second point of the debug edge generated for the neighbour of
				//the current edge (edgeGraph[i]) must be the one opposite point
				//of the neighbour edge from the one that connects to the current edge
				if (e.a == neighbourEdge.a)
					p2 = getPointOnLine(p1,neighbourEdge.b, relativeEndOffset);
				else
				if (e.a == neighbourEdge.b)
					p2 = getPointOnLine(p1,neighbourEdge.a, relativeEndOffset);

				//add the debug edge to the rest of the data
				temp.a = p1;
				temp.b = p2;
				debugData.push_back(temp);
			}
		}

		//do not add debug data for disabled edges
		if (e.enabled)
		{
			//construct debug data for all neighbours of point b of edge e
			p1 = getPointOnLine(e.a,e.b, 1-relativeStartOffset);
			//iterate over all the neighbours of point b; meaning bnb vector
			//for (mapIter = e.bnb.begin();mapIter != e.bnb.end();++mapIter)
			//nu mai avem nevoie de iteratie. Doar 1 nb
			//for (int i = 0;i<e.bnb.size();i++)
			if (e.bnb != -1)
			{
				cml::Edge neighbourEdge;
				//neighbourEdge = edgeGraph[mapIter->first];
				//neighbourEdge = edgeGraph[e.bnb[i]];
				neighbourEdge = edgeGraph[e.bnb];

				if (e.b == neighbourEdge.a)
					p2 = getPointOnLine(p1,neighbourEdge.b, relativeEndOffset);
				else
				if (e.b == neighbourEdge.b)
					p2 = getPointOnLine(p1,neighbourEdge.a, relativeEndOffset);

				temp.a = p1;
				temp.b = p2;
				debugData.push_back(temp);
			}
		}
	}	
}

cml::Vector2f cml::CityCell::getPointOnLine(cml::Vector2f p1, cml::Vector2f p2,float offset)
{
	cml::Vector2f res;

	res.x = p1.x + offset*(p2.x-p1.x);
	res.y = p1.y + offset*(p2.y-p1.y);

	return res;
}

float cml::CityCell::getPointOffset(cml::Vector2f p1, cml::Vector2f p2,cml::Vector2f point)
{
	if (p1.x != p2.x)
		return (point.x - p1.x)/(p2.x-p1.x);
	else
		return -1;
}

void cml::CityCell::lineGraphMerge()
{	
	for (int i=0;i<lineGraph.size();i++)
	{
		polygonIntersect(lineGraph[i]);
	}	
}

void cml::CityCell::polygonIntersect(cml::Edge const &line)
{	
	//the two points of the first edge of the lineGraph
	cml::Vector2f p1,p2;
	p1 = line.a;
	p2 = line.b;

	//number of intersections for each edge
	int numLineIntersect = 0;
	bool p1Outside,p2Outside;
	
	std::vector<int>		   intersectionIndexArray;
	std::vector<cml::Vector2f> intersectionPositionArray;

	p1Outside = p2Outside = true;

	for (unsigned int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e;
		cml::Vector2f intersectPoint;

		e = edgeGraph[i];	

		if (e.enabled && e.partOfEdgeGraph)
		{
			if (lineIntersect(e.a,e.b,p1,p2,intersectPoint) == 1)
			{
				numLineIntersect++;
				//memorize the intersection position and the edge
				//(from the edgeGraph) with which the intersection occured
				intersectionIndexArray.push_back(i);
				intersectionPositionArray.push_back(intersectPoint);
			}
		}
	}

p1Outside = !pointInPolygon(p1);
p2Outside = !pointInPolygon(p2);

//no intersections have occured
if (intersectionIndexArray.size() == 0)
{
	//both points are outside
	if (p1Outside && p2Outside)
	{
		//discard the current edge (of the lineGraph)		
	}

	//both points are inside
	if (!p1Outside && !p2Outside)
	{
		cml::Edge temp(p1,p2);
		//merged from lineGraph
		temp.partOfEdgeGraph = false;
		edgeGraph.push_back(temp);		
	}
}

//one intersection
if (intersectionIndexArray.size() == 1)
{
	if (p1Outside && p2Outside)
	{
		//an error or extreme case occured
		//salvage what you can - i.e. do dot perform intersections
		//de debug
		std::cout<<"outside x2 intersect x1 ERROR\n";
	}
	else
	{
		//if the first point is outside clip the line 
		//from the interior point to the intersection point
		//the only other case at this point is that p1 -int / p2 - ext
		if (p1Outside)
		{
			lineMerge(intersectionPositionArray[0],p2,intersectionIndexArray[0]);
		}
		else
		{
			lineMerge(intersectionPositionArray[0],p1,intersectionIndexArray[0]);
		}
	}
}

//a list of intersections
if (intersectionIndexArray.size() >= 2)
{
	int start,stop,size;

	start = 0;
	//index for last element = arraySize - 1
	size = intersectionIndexArray.size()-1;
	stop = size;

	if (size > 0) sortPointsOnLine(p1,p2,intersectionPositionArray,intersectionIndexArray);

	if (!p1Outside)
	{
		lineMerge(intersectionPositionArray[0],p1,intersectionIndexArray[0]);
		//because the first intersection point has been processed
		start = 1;
	}

	if (!p2Outside)
	{
		lineMerge(intersectionPositionArray[size],p2,intersectionIndexArray[size]);
		//because the last intersection point has been processed
		stop -= 1;
	}

	while (start < stop)
	{
		lineMerge(intersectionPositionArray[start],
				  intersectionPositionArray[start+1],
				  intersectionIndexArray[start],
				  intersectionIndexArray[start+1]);
		start += 2;
	}
		
}

}

bool cml::CityCell::pointInPolygon(cml::Vector2f p)
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

//TODO: change name from lineMerge to something else more appropiate
void cml::CityCell::lineMerge(cml::Vector2f intersectPos1,cml::Vector2f intersectPos2,
		int edgeIndex1,int edgeIndex2,int intersectType1,int intersectType2)
{
	cml::Edge e,temp;
	//positions in the array for the line and generated edges
	int lpos,e1pos,e2pos;
	std::map<int,int>::const_iterator mapIter;

	//get the intersected edge
	e = edgeGraph[edgeIndex1];
	//and disable it
	edgeGraph[edgeIndex1].enabled = false;
	edgeGraph[edgeIndex1].anb = -1;
	edgeGraph[edgeIndex1].bnb = -1;

	//add the current edge (from the lineGraph) to the edgeGraph
	//do not add the existing neighbours (links to other edges in lineGraph)
	//(if exits, implemented) yet so a temporary edge without neighbours 
	//must be constructed the edge must be 'clipped' to the current cell 
	//based on the interior/exterior positions of the two points

	//add the current edge to the edgeGraph
	temp.a = intersectPos1;
	temp.b = intersectPos2;
	//merged from lineGraph
	temp.partOfEdgeGraph = false;
	edgeGraph.push_back(temp);
	//record the position in the array of the line
	lpos = edgeGraph.size()-1;
	temp.partOfEdgeGraph = true;

	//construct two new edges from point a and b of
	//the original point and the intersection point
	temp.a = e.a;
	temp.b = intersectPos1;
	edgeGraph.push_back(temp);
	//record the position in the array of the first constructed edge
	e1pos = edgeGraph.size()-1;

	temp.a = intersectPos1;
	temp.b = e.b;
	edgeGraph.push_back(temp);
	//record the position in the array of the first constructed edge
	e2pos = edgeGraph.size()-1;

	//TODO: different terminology than current,generated 1&2
	//now add neighbours for the constructed edges
	//to access:
	//		current edge		- size()-3
	//		generated edge 1	- size()-2
	//		generated edge 2	- size()-1

	//also check if edgeIndex2 != -1. This means that two intersections
	//must pe processed.
	if (edgeIndex2 != -1)
	{
		//the second edge
		e = edgeGraph[edgeIndex2];
		//and disable it
		edgeGraph[edgeIndex2].enabled = false;
		edgeGraph[edgeIndex2].anb = -1;
		edgeGraph[edgeIndex2].bnb = -1;


		//the edge corresponding to the line has already been added
		//...

		//add two new edges - the same as for intersectPos1
		temp.a = e.a;
		temp.b = intersectPos2;
		edgeGraph.push_back(temp);		
		e1pos = edgeGraph.size()-1;

		temp.a = intersectPos2;
		temp.b = e.b;
		edgeGraph.push_back(temp);		
		e2pos = edgeGraph.size()-1;
	}
}

void cml::CityCell::sortPointsOnLine(cml::Vector2f p1, cml::Vector2f p2,std::vector<cml::Vector2f>& positionArray,
													std::vector<int>& indexArray)
{

	for(int i=0;i<indexArray.size()-1;i++)
	{
		int minIndex = i;
		float minOffset = getPointOffset(p1,p2,positionArray[i]);

		for (int j=i+1;j<indexArray.size();j++)
		{
			if (getPointOffset(p1,p2,positionArray[j]) < minOffset)
			{
				minIndex = j;
				minOffset = getPointOffset(p1,p2,positionArray[j]);
			}
		}

		if (minIndex != i)
		{
			int tempIndex = indexArray[i];
			indexArray[i] = indexArray[minIndex];
			indexArray[minIndex] = tempIndex;

			cml::Vector2f tempPosition = positionArray[i];
			positionArray[i] = positionArray[minIndex];
			positionArray[minIndex] = tempPosition;
		}
	
	}

	//de debug
	/*
	for(int i=0;i<indexArray.size();i++)
	{
		std::cout<<indexArray[i]<<" "<<positionArray[i].x<<" "<<positionArray[i].y<<" | "<<
			getPointOffset(p1,p2,positionArray[i])<<std::endl;
	}
	*/
}

void cml::CityCell::calculateSubcells()
{	
	lineGraph.clear();

	for (int i=0;i<edgeGraph.size();i++)
	{
		int v1Index,v2Index,foundIndex;
		bool exist;

		if (edgeGraph[i].enabled)
		{
			//pentru vertexul 'a' al cml::Edge
			exist = false;
			for (int j=0;j<vertices.size();j++)
			{
				if ((vertices[j].X() == edgeGraph[i].a.x)
					&&
					(vertices[j].Y() == edgeGraph[i].a.y))
				{
					exist = true; foundIndex = j; break;
				}
			}

			if (!exist)
			{
				vertices.push_back(FVector(edgeGraph[i].a.x,edgeGraph[i].a.y));
				v1Index = vertices.size()-1;			
			}
			else
			{
				v1Index = foundIndex;
			}

			//pentru vertexul 'b' al cml::Edge
			exist = false;
			for (int j=0;j<vertices.size();j++)
			{
				if ((vertices[j].X() == edgeGraph[i].b.x)
					&&
					(vertices[j].Y() == edgeGraph[i].b.y))
				{
					exist = true; foundIndex = j; break;
				}
			}

			if (!exist)
			{
				vertices.push_back(FVector(edgeGraph[i].b.x,edgeGraph[i].b.y));
				v2Index = vertices.size()-1;			
			}
			else
			{
				v2Index = foundIndex;
			}

			edges.push_back(IVector(v1Index,v2Index));
		}
	}

//insereaza vertecstii in graful pentru MCB
for (int i=0;i<vertices.size();i++)
{
	graph.InsertVertex(vertices[i],i);
}

//insereaza muchiile in graful pentru MCB
for (int i=0;i<edges.size();i++)
	graph.InsertEdge(edges[i].X(),edges[i].Y());

int PQuantity;
graph.ExtractPrimitives(primitives);
PQuantity = (int)primitives.size();

for (int i=0;i<PQuantity;i++)
{
	Wm4::PlanarGraph<FVector>::Primitive* primitive;
	primitive = primitives[i];
	
	int SQuantity;
	SQuantity = (int)primitive->Sequence.size();

	if (primitive->Type == Wm4::PlanarGraph<FVector>::PT_MINIMAL_CYCLE)
	{
		FVector elem1,elem2;
		cml::Edge e;
		cml::Subcell s;
		
		for (int j=0;j<SQuantity-1;j++)
		{
			elem1 = primitive->Sequence[j].first;
			elem2 = primitive->Sequence[j+1].first;

			e.a = cml::Vector2f(elem1.X(),elem1.Y());
			e.b = cml::Vector2f(elem2.X(),elem2.Y());
			
			if (j==0)			
				e.anb = SQuantity-1;
			else
				e.anb = j-1;
			e.bnb = j+1;			

			//la trecerea de la edgeGraph -> WM4 pentru gasirea de subcelule
			//s-au pierdut informatii, anume daca tine sau nu edge-ul de 
			//peretii celulei. Trebuie regasite.
			e.partOfEdgeGraph = false;
			for (int k=0;k<edgeGraph.size();k++)
			{
				if (edgeGraph[k].partOfEdgeGraph)
				{
					if (edgeGraph[k] == e)
					{
						e.partOfEdgeGraph = true;
						break;
					}
				}
			}
			s.edges.push_back(e);
			//de debug
			lineGraph.push_back(e);
			s.bbox.addLine(e.a,e.b);
		}

		elem1 = primitive->Sequence[SQuantity-1].first;
		elem2 = primitive->Sequence[0].first;

		e.a = cml::Vector2f(elem1.X(),elem1.Y());
		e.b = cml::Vector2f(elem2.X(),elem2.Y());

		e.anb = SQuantity-2;
		e.bnb = 0;

		//la trecerea de la edgeGraph -> WM4 pentru gasirea de subcelule
		//s-au pierdut informatii, anume daca tine sau nu edge-ul de 
		//peretii celulei. Trebuie regasite.
		e.partOfEdgeGraph = false;
		for (int k=0;k<edgeGraph.size();k++)
		{
			if (edgeGraph[k].partOfEdgeGraph)
			{
				if (edgeGraph[k] == e)
				{
					e.partOfEdgeGraph = true;
					break;
				}
			}
		}
		s.edges.push_back(e);
		//de debug
		lineGraph.push_back(e);
		s.bbox.addLine(e.a,e.b);

		subcells.push_back(s);
	}
}

}



void cml::CityCell::scaleSubcells(float offset)
{
cml::Subcell s;

lineGraph.clear();
for (int k=0;k<subcells.size();k++)
{
	s = subcells[k];

	for (int i=0;i<s.edges.size();i++)
	{
		cml::Edge e,ea,eb;
		cml::Vector2f normal,middle;

		e	= s.edges[i];
		ea	= s.edges[e.anb];
		eb	= s.edges[e.bnb];
		
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
		normal *= offset;
		//pentru punctul de mijloc
		//middle = e.a + e.b;
		//middle = middle.midpoint();	
		//pentru normala la mijlocul segmentului
		//lineGraph.push_back(cml::Edge(middle,middle+normal));

		e.a = e.a + normal;
		e.b = e.b + normal;

		normal = ea.b - ea.a;
		normal = normal.perpendicular();
		normal = normal.normalize();
		normal *= offset;
		ea.a = ea.a + normal;
		ea.b = ea.b + normal;

		normal = eb.b - eb.a;
		normal = normal.perpendicular();
		normal = normal.normalize();
		normal *= offset;
		eb.a = eb.a + normal;
		eb.b = eb.b + normal;

		cml::Vector2f p1,p2;

		lineIntersect(e.a,e.b,ea.a,ea.b,p1);
		lineIntersect(e.a,e.b,eb.a,eb.b,p2);

		e.a = p1;
		e.b = p2;

		//de debug
		lineGraph.push_back(e);
	}
}	//END k loop
}

void cml::CityCell::constructRoadGeometry(float offset, float texRepeatsPerUnit)
{
	//TODO: datorita directiei, daca generam drumuri pentru doua subcelule alaturate
	//cele doua bucati for fi oarecum oglindite. O metoda de rezolvare e constructia
	//texturii drumului in asa fel incat sa nu se observe. Alta metoda (mai buna)
	//ar fi sincronizarea directiilor la edge-uri. Dar e mai complicat de implementat.
	//deci va fi folosita deocamdata metoda 1.
	std::vector<cml::Edge> scaledEdgeGraph;

	for (int i=0;i<edgeGraph.size();i++)
	{
		cml::Edge e,eaneighbour,ebneighbour;
		cml::Vector2f normal,middle;

		e = edgeGraph[i];		
		eaneighbour	= edgeGraph[e.anb];
		ebneighbour	= edgeGraph[e.bnb];
		
		if (!e.partOfEdgeGraph)
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
			normal *= offset;
			//pentru punctul de mijloc
			//middle = e.a + e.b;
			//middle = middle.midpoint();	
			//pentru normala la mijlocul segmentului
			//lineGraph.push_back(cml::Edge(middle,middle+normal));

			e.a = e.a + normal;
			e.b = e.b + normal;
		}

		if (!eaneighbour.partOfEdgeGraph)
		{
			normal = eaneighbour.b - eaneighbour.a;
			normal = normal.perpendicular();
			normal = normal.normalize();
			normal *= offset;
			eaneighbour.a = eaneighbour.a + normal;
			eaneighbour.b = eaneighbour.b + normal;
		}

		if (!ebneighbour.partOfEdgeGraph)
		{
			normal = ebneighbour.b - ebneighbour.a;
			normal = normal.perpendicular();
			normal = normal.normalize();
			normal *= offset;
			ebneighbour.a = ebneighbour.a + normal;
			ebneighbour.b = ebneighbour.b + normal;
		}

		cml::Vector2f p1,p2;

		lineIntersect(e.a,e.b,eaneighbour.a,eaneighbour.b,p1);
		lineIntersect(e.a,e.b,ebneighbour.a,ebneighbour.b,p2);

		p1.x = (float)((int)(p1.x*100))/100;
		p1.y = (float)((int)(p1.y*100))/100;
		p2.x = (float)((int)(p2.x*100))/100;
		p2.y = (float)((int)(p2.y*100))/100;

		e.a = p1;
		e.b = p2;

		normal = p1 - p2;
		normal = normal.perpendicular();
		normal = normal.normalize();
		normal *= -offset;
		p1 = p1 - normal;
		p2 = p2 - normal;			

		float divisionPoint1,divisionPoint2;
		divisionPoint1 = getPointOffset(edgeGraph[i].a,edgeGraph[i].b,p1);			
		divisionPoint2 = getPointOffset(edgeGraph[i].a,edgeGraph[i].b,p2);

		float total;
		float edgeLength;

		edgeLength = (edgeGraph[i].a - edgeGraph[i].b).length();
		total = edgeLength*texRepeatsPerUnit;
		total = (int)total+0.004;
		
		if (!edgeGraph[i].partOfEdgeGraph)
		{
			cml::Triangle3f t;

			float coord1,coord2;
			
			coord1 = total - divisionPoint1*total;
			coord2 = total - divisionPoint2*total;


			t.a = cml::Vector3f(edgeGraph[i].b.x,0,edgeGraph[i].b.y);
			t.b = cml::Vector3f(edgeGraph[i].a.x,0,edgeGraph[i].a.y);			
			t.c = cml::Vector3f(e.a.x,0,e.a.y);
			t.atcoord = cml::Vector2f(0,0);
			t.btcoord = cml::Vector2f(0,total);
			t.ctcoord = cml::Vector2f(1,coord1);
			t.material = "red";
			roadGeometry.push_back(t);

			t.a = cml::Vector3f(e.a.x,0,e.a.y);
			t.b = cml::Vector3f(e.b.x,0,e.b.y);
			t.c = cml::Vector3f(edgeGraph[i].b.x,0,edgeGraph[i].b.y);
			t.atcoord = cml::Vector2f(1,coord1);
			t.btcoord = cml::Vector2f(1,coord2);
			t.ctcoord = cml::Vector2f(0,0);
			t.material = "red";
			roadGeometry.push_back(t);
		}

		scaledEdgeGraph.push_back(e);
	}

	for (int i=0;i<edgeGraph.size();i++)
	{
		scaledEdgeGraph[i].partOfEdgeGraph = true;
		edgeGraph[i] = scaledEdgeGraph[i];
	}
}

void cml::CityCell::addEdge(cml::Edge e, bool updateBBox)
{
	edgeGraph.push_back(e);
	bbox.addLine(e.a,e.b);
};

void cml::CityCell::addLine(cml::Edge e)
{
	lineGraph.push_back(e);
};

void cml::CityCell::calculateAreaAndCentroid(bool onlyBoundary)
{
	//from here:
	//http://local.wasp.uwa.edu.au/~pbourke/geometry/polyarea/

	/*
	do not recalculate area and centroid if they are valid
	workflow:
		- cell generated
		- one generator calls calculateAreaAndCentroid
		- area now valid ( != -1)
		- another generator calls -> do nothing
	*/

	if (area != -1)
	{
		std::cout<<"area already calculated: "<<area<<"\n";
	}
	else
	{	
		std::vector<cml::Vector2f> vertices;
		std::vector<cml::Vector2f>::iterator vertIter;

		//find all the unique vertices of the cell boundary
		if (edgeGraph.size() > 0)
		{
			for (int i=0;i<edgeGraph.size();i++)
			{
				Edge e = edgeGraph[i];

				if (e.enabled)
				{
					if (!onlyBoundary)
					{
						if (e.partOfEdgeGraph)
						{
							//add the first point of the edge (if not already added)
							vertIter = std::find(vertices.begin(),vertices.end(), e.a);
							if (vertIter == vertices.end())
							{
								vertices.push_back(e.a);
							}

							//add the second point of the edge (if not already added)
							vertIter = std::find(vertices.begin(),vertices.end(), e.b);
							if (vertIter == vertices.end())
							{
								vertices.push_back(e.b);
							}
						}
					}
					else
					{
						//add the first point of the edge (if not already added)
						vertIter = std::find(vertices.begin(),vertices.end(), e.a);
						if (vertIter == vertices.end())
						{
							vertices.push_back(e.a);
						}

						//add the second point of the edge (if not already added)
						vertIter = std::find(vertices.begin(),vertices.end(), e.b);
						if (vertIter == vertices.end())
						{
							vertices.push_back(e.b);
						}
					}
				}
			}
			//last vertex == first vertex (closed polygon)
			vertices.push_back(vertices[0]);
		}


		//calculate the area
		if (vertices.size() > 0)
		{
			for (int i=0;i<vertices.size()-1;i++)
			{
				float temp;

				temp = vertices[i].x*vertices[i+1].y - vertices[i+1].x*vertices[i].y;
				area += temp;
				
				centroid.x += (vertices[i].x + vertices[i+1].x)*temp;
				centroid.y += (vertices[i].y + vertices[i+1].y)*temp;
			}
			area = area / 2;

			if (area != 0)
			{
				centroid.x = centroid.x / (6*area);
				centroid.y = centroid.y / (6*area);
			}

			//surface must be positive
			//but the sign of the area can be used
			//to determine the ordering of the vertices - cw or ccw
			if (area < 0) area = -area;
		}
		
		//de debug
		//std::cout<<"area "<<area<<"\n";
		//addEdge(cml::Edge(centroid,centroid + cml::Vector2f(10,10)));
	}// END large else branch
}