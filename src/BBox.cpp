#include "BBox.h"

cml::BBox::BBox()
{
	max = cml::Vector2f(1,1);
	min  = cml::Vector2f(-1,-1);
}

cml::BBox::BBox(cml::Vector2f maximum, cml::Vector2f minimum)
{
	max = maximum;
	min = minimum;
	//if min > max
	repair();
}

cml::BBox::~BBox()
{

}

void cml::BBox::addLine(cml::Vector2f p1, cml::Vector2f p2)
{
	//first line to be added
	if ((max.x == 1) && (max.y == 1) && (min.x == -1) && (min.y == -1))
	{
		if (p1.x < p2.x)
		{
			max.x = p2.x;
			min.x = p1.x;
		}
		else
		{
			max.x = p1.x;
			min.x = p2.x;
		}

		if (p1.y < p2.y)
		{
			max.y = p2.y;
			min.y = p1.y;
		}
		else
		{
			max.y = p1.y;
			min.y = p2.y;
		}
	}
	//add lines normally	
	else
	{
		if (p1.x < min.x) min.x = p1.x;
		if (p1.y < min.y) min.y = p1.y;
		if (p1.x > max.x) max.x = p1.x;
		if (p1.y > max.y) max.y = p1.y;

		if (p2.x < min.x) min.x = p2.x;
		if (p2.y < min.y) min.y = p2.y;
		if (p2.x > max.x) max.x = p2.x;
		if (p2.y > max.y) max.y = p2.y;		
	}
}

void cml::BBox::addPadding(float padding)
{
	max += padding;
	min  -= padding;
}

void cml::BBox::repair()
{
	float t;

	if (min.x > max.x) {t = max.x; max.x = min.x; min.x = t; };
	if (min.y > max.y) {t = max.y; max.y = min.y; min.y = t; };	
}