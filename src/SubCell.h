#ifndef SUBCELL_H
#define SUBCELL_H

#include "BBox.h"
#include "Edge.h"

namespace cml
{

class Subcell
{
public:
	std::vector<cml::Edge> edges;
	cml::BBox bbox;
};

}

#endif