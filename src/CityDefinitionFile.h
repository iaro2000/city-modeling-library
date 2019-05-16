#ifndef CITYDEFINITIONFILE_H
#define CITYDEFINITIONFILE_H

#include "iostream"
#include "vector"

namespace cml
{

struct CityDefinitionFile
{
	//general section
	unsigned int citySeed;
	std::string initialCellBoundary;
	std::string initialRoadNetwork;
	
	//std::vector - for the array of levels
	std::vector< bool > bboxVisibility;
	std::vector< int > randValLimit;
	std::vector< std::string > roadParameters;

	std::vector< std::vector< std::string > > ruleConditions;
	std::vector< std::vector< std::string > > ruleActions;


};

}

#endif