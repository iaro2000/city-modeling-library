#ifndef VECTOR3_H
#define VECTOR3_H

namespace cml
{

//3D vector (for geometry)
//doar schelet
//de dezvoltat doar cand am nevoie de Vector3
template <class T>
class templateVector3
{
public:
	templateVector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	templateVector3(T xval, T yval,T zval)
	{
		x = xval;
		y = yval;
		z = zval;
	}

	T x;
	T y;
	T z;
};

typedef templateVector3<float> Vector3f;

}

#endif