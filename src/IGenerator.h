#ifndef IGENERATOR_H
#define IGENERATOR_H

#include "CityCell.h"
#include "iostream"

namespace cml
{

class IGenerator
{
public:
	virtual void setTarget(CityCell *c) =0;
	virtual void setParameters(std::string p) =0;
	virtual void generate() =0;
	virtual void reset() =0;
protected:
	CityCell *target;
};

}

#endif