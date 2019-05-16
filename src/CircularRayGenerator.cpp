#include "CircularRayGenerator.h"

cml::CircularRayGenerator::CircularRayGenerator()
{

}

cml::CircularRayGenerator::~CircularRayGenerator()
{

}

void cml::CircularRayGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::CircularRayGenerator::setParameters(std::string p)
{
	//Parameters:
	//a - reference (own bbox, that of parent, that of root)
	//b c - minimum / maximum nr of rays
	//d e - minimum / maximum angle difference between succesive rays

	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	std::string referenceBBox;

	ss<<p;
	ss>>referenceBBox;

	if (referenceBBox == "self")
	{
		bbox = target->getBoundingBox();
		target->calculateAreaAndCentroid();
		centerPoint = target->getCentroid();
	}

	ss>>minRays>>maxRays>>minAngle>>maxAngle;
}

void cml::CircularRayGenerator::generate()
{
	float rayLength;
	float lastAngle;

	//check which dimension of the bbox is larger
	//and set the rayLength as larger than that.
	//also rayLength does no need to be divided in two.
	//works because lines in the lineGraph will
	//be clipped to the polygon anyway.
	if (bbox.getLength() > bbox.getWidth())
	{
		rayLength = bbox.getLength();
	}
	else
	{
		rayLength = bbox.getWidth();
	}
	
	lastAngle = 0;
	for (int i=0;i<minRays + rand()%maxRays;i++)
	{
		float angle,x,y;
		angle = lastAngle + minAngle + rand()%maxAngle;
		
		if (angle > 360 ) break;

		x = centerPoint.x + sin(angle * 3.141592/180)*rayLength;
		y = centerPoint.y + cos(angle * 3.141592/180)*rayLength;

		target->addLine(cml::Edge(centerPoint,cml::Vector2f(x,y)));

		lastAngle = angle;
	}	
}

void cml::CircularRayGenerator::reset()
{

}