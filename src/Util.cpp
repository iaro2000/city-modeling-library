#include "Util.h"

cml::Util* cml::Util::instancePointer = 0;

cml::Util::Util()
{

}

cml::Util::~Util()
{
	if (instancePointer != 0)
	{
		delete instancePointer;
	}
}

cml::Util* cml::Util::instance()
{
	if (instancePointer == 0)
	{
		instancePointer = new Util();
	}
	return instancePointer;
}


