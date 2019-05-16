#include "GridGenerator.h"

cml::GridGenerator::GridGenerator()
{

}

cml::GridGenerator::~GridGenerator()
{

}

void cml::GridGenerator::setTarget(CityCell *c)
{
	target = c;
}

void cml::GridGenerator::setParameters(std::string p)
{
	//Parameters:
	//a - reference (own bbox, that of parent, that of root)
	//b c d e - minimum / maximum | length / width percent distance
	//for vertical/horizontal line distance
	//h i - width/length offsets (in percent) for grid rotation

	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	std::string referenceBBox;

	ss<<p;
	ss>>referenceBBox;

	if (referenceBBox == "self")
	{
		bbox = target->getBoundingBox();
	}

	ss>>minPercentLength>>maxPercentLength>>minPercentWidth>>maxPercentWidth;
	ss>>minAngle>>maxAngle;
}

void cml::GridGenerator::generate()
{
	cml::Vector2f originalBboxCenter;
	originalBboxCenter.x = (bbox.max.x + bbox.min.x)/2;
	originalBboxCenter.y = (bbox.max.y + bbox.min.y)/2;

	float paddingSize;
	paddingSize = sqrt(bbox.getLength()*bbox.getLength()/4+bbox.getWidth()*bbox.getWidth()/4);
	//if (bbox.getLength() > bbox.getWidth()) paddingSize -= bbox.getLength()/4;
	//else									paddingSize -= bbox.getWidth()/4;
	bbox.addPadding(paddingSize);

	float minx = bbox.min.x;
	float miny = bbox.min.y;
	float maxx = bbox.max.x;
	float maxy = bbox.max.y;

	float upperLimitX,upperLimitY;
	float lowerLimitX,lowerLimitY;
	float length,width;

	float currentX,currentY;
	bool stopWidth,stopLength;

	std::vector<float> widthArray,lengthArray;
	std::vector<std::vector<cml::Vector2f>> pointArray;

	currentX = minx;
	currentY = miny;
	stopWidth = stopLength = false;
				
	length = abs(maxx-minx);
	width  = abs(maxy-miny);

	lowerLimitX = length * (minPercentLength/100);
	lowerLimitY = width  * (minPercentWidth /100);
	upperLimitX = length * (maxPercentLength/100);
	upperLimitY = width  * (maxPercentLength/100);
	
	widthArray.push_back(currentY);
	lengthArray.push_back(currentX);

	while (!stopWidth)
	{
		cml::Vector2f p1, p2;
		p1.x = minx;
		p2.x = maxx;
		float widthR = currentY + lowerLimitY + (float)(rand()%((int)upperLimitY*1000))/1000;
		p1.y = widthR;
		p2.y = widthR;

		if (widthR < maxy)
		{			
			widthArray.push_back(widthR);
			currentY = widthR;
		}
		else
			stopWidth = true;
	}

	while (!stopLength)
	{
		cml::Vector2f p1, p2;
		p1.y = miny;
		p2.y = maxy;
		float lengthR = currentX + lowerLimitX + (float)(rand()%((int)upperLimitX*1000))/1000;
		p1.x = lengthR;
		p2.x = lengthR;

		if (lengthR < maxx)
		{
			lengthArray.push_back(lengthR);
			currentX = lengthR;
		}
		else
			stopLength = true;
	}

	widthArray.push_back(maxy);
	lengthArray.push_back(maxx);

	cml::Vector2f v1,v2,v3,v4,vt;
	float angleSin,angleCos;
	float randAngle;

	randAngle = minAngle + (float)(rand()%((int)(maxAngle*1000))/1000);

	angleSin = sin(randAngle*3.14159/180);
	angleCos = cos(randAngle*3.14159/180);
	
	for (int i=1;i<widthArray.size();i++)
	{
		for (int j=1;j<lengthArray.size();j++)
		{


			v1 = cml::Vector2f(lengthArray[j],widthArray[i-1]);
			v2 = cml::Vector2f(lengthArray[j],widthArray[i]);
			v3 = cml::Vector2f(lengthArray[j-1],widthArray[i]);
			v4 = cml::Vector2f(lengthArray[j],widthArray[i]);

			vt = v1; v1 -= originalBboxCenter;
			vt.x = v1.x * angleCos - v1.y * angleSin;
			vt.y = v1.x * angleSin + v1.y * angleCos;
			v1 = vt; v1 += originalBboxCenter;

			vt = v2; v2 -= originalBboxCenter;
			vt.x = v2.x * angleCos - v2.y * angleSin;
			vt.y = v2.x * angleSin + v2.y * angleCos;
			v2 = vt; v2 += originalBboxCenter;

			vt = v3; v3 -= originalBboxCenter;
			vt.x = v3.x * angleCos - v3.y * angleSin;
			vt.y = v3.x * angleSin + v3.y * angleCos;
			v3 = vt; v3 += originalBboxCenter;

			vt = v4; v4 -= originalBboxCenter;
			vt.x = v4.x * angleCos - v4.y * angleSin;
			vt.y = v4.x * angleSin + v4.y * angleCos;
			v4 = vt; v4 += originalBboxCenter;

			target->addLine(cml::Edge(v1,v2));
			target->addLine(cml::Edge(v3,v4));
		}
	}

	widthArray.clear();
	lengthArray.clear();
	pointArray.clear();
}

void cml::GridGenerator::reset()
{

}