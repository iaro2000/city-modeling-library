#ifndef UTIL_H
#define UTIL_H

#include "iostream"
#include "string"

namespace cml
{

class Util
{
public:
	static Util* instance();
	~Util();
private:
	Util();
	static Util* instancePointer;
};

}

#endif