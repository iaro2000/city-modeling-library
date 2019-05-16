#include "QuadDivisionGenerator.h"

cml::QuadDivisionGenerator::QuadDivisionGenerator()
{

}

cml::QuadDivisionGenerator::~QuadDivisionGenerator()
{

}

void cml::QuadDivisionGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::QuadDivisionGenerator::setParameters(std::string p)
{
	//Parameters:
	//a - reference (own bbox, that of parent, that of root)
	//b - number of iterations
	//c d e f - minimum / maximum | length / width percent distance
	//of current division rectangle walls from which to start.
	//g - minimal surface area (in percent of origional bbox) for
	//which subdivision can occur

	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	std::string referenceBBox;

	ss<<p;
	ss>>referenceBBox;

	if (referenceBBox == "self")
	{
		bbox = target->getBoundingBox();
		target->calculateAreaAndCentroid();		
		area = target->getArea();
	}

	ss>>iterations;
	ss>>minPercentLength>>maxPercentLength>>minPercentWidth>>maxPercentWidth;
	ss>>minArea;
	minArea = (minArea / 100) * area;
}

void cml::QuadDivisionGenerator::generate()
{
	bbox.addPadding(1);
	divide(iterations,bbox.min.x,bbox.min.y,bbox.max.x,bbox.max.y);
}

void cml::QuadDivisionGenerator::divide(int iterations, 
										float minx, float miny, float maxx, float maxy)
{
	Edge line;
	std::vector<cml::Edge>::iterator lineIter;
	std::vector<cml::Edge> lineGraph;	

	if ((iterations > 0) /*&& (abs(maxx-minx)*abs(maxy-miny) >= minArea)*/)
	{
		float rx,ry;
		float upperLimitX,upperLimitY;
		float lowerLimitX,lowerLimitY;
		float length,width;
				
		length = abs(maxx-minx);
		width  = abs(maxy-miny);

		lowerLimitX = length * (minPercentLength/100);
		lowerLimitY = width  * (minPercentWidth /100);
		upperLimitX = length * (maxPercentLength/100);
		upperLimitY = width  * (maxPercentWidth/100);
		
		length = length - lowerLimitX - upperLimitX;
		width  = width  - lowerLimitY - upperLimitY;
		length = length * 1000;		//for use with rand
		width  = width  * 1000;
		if (length != 0) {length = rand()%((int)length);};
		if (width  != 0) {width  = rand()%((int)width );};
		length = length / 1000;
		width  = width  / 1000;

		rx = minx + lowerLimitX + 0 + length;
		ry = miny + lowerLimitY + 0 + width;

		divide(iterations-1,minx,miny,rx,ry);
		divide(iterations-1,rx,miny,maxx,ry);
		divide(iterations-1,minx,ry,rx,maxy);
		divide(iterations-1,rx,ry,maxx,maxy);
	}
	else
	{	
		/*
		target->addLine(cml::Edge(cml::Vector2f(minx,miny),cml::Vector2f(maxx,miny)));
		target->addLine(cml::Edge(cml::Vector2f(maxx,miny),cml::Vector2f(maxx,maxy)));
		*/
		
		line = cml::Edge(cml::Vector2f(minx,miny),cml::Vector2f(maxx,miny));
		lineIter = std::find(lineGraph.begin(),lineGraph.end(), line);
		if (lineIter == lineGraph.end())
		{
			target->addLine(line);
		}
		lineGraph = target->getLineGraphData();
		
		
		line = cml::Edge(cml::Vector2f(maxx,miny),cml::Vector2f(maxx,maxy));
		lineIter = std::find(lineGraph.begin(),lineGraph.end(), line);
		if (lineIter == lineGraph.end())
		{
			target->addLine(line);
		}
		lineGraph = target->getLineGraphData();
		
		/*
		line = cml::Edge(cml::Vector2f(maxx,maxy),cml::Vector2f(minx,maxy));
		lineIter = std::find(lineGraph.begin(),lineGraph.end(), line);
		if (lineIter == lineGraph.end())
		{
			target->addLine(line);
		}
		lineGraph = target->getLineGraphData();
		
		
		line = cml::Edge(cml::Vector2f(minx,maxy),cml::Vector2f(minx,miny));
		lineIter = std::find(lineGraph.begin(),lineGraph.end(), line);
		if (lineIter == lineGraph.end())
		{
			target->addLine(line);
		}
		lineGraph = target->getLineGraphData();
		*/
		
		/*
		target->addLine(cml::Edge(cml::Vector2f(minx,miny),cml::Vector2f(maxx,miny)));
		target->addLine(cml::Edge(cml::Vector2f(maxx,miny),cml::Vector2f(maxx,maxy)));
		target->addLine(cml::Edge(cml::Vector2f(maxx,maxy),cml::Vector2f(minx,maxy)));
		target->addLine(cml::Edge(cml::Vector2f(minx,maxy),cml::Vector2f(minx,miny)));
		*/
	}
}

void cml::QuadDivisionGenerator::reset()
{

}