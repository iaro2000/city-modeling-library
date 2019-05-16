#ifndef PREFAB_H
#define PREFAB_H

#include "iostream"

namespace cml
{

class Prefab
{
public:
	std::string resourceName;
	cml::Vector3f position;
	cml::Vector3f rotation;
	cml::Vector3f scale;
};

}

#endif