#ifndef VECTOR2_H
#define VECTOR2_H

namespace cml
{
	//2D vector
template <class T>
class templateVector2
{
public:

	templateVector2()
	{
		x = 0;
		y = 0;
	}

	templateVector2(T xval, T yval)
	{
		x = xval;
		y = yval;
	}

	bool operator==(const templateVector2& v) const
	{
		return ((v.x == x) && (v.y == y));
	}

	bool operator!=(const templateVector2& v) const
	{
		return ((v.x != x) || (v.y != y));
	}

	templateVector2 operator+(const templateVector2& v)
	{
		templateVector2 temp;
		temp.x = x + v.x;
		temp.y = y + v.y;

		return temp;
	}

	templateVector2 operator-(const templateVector2& v)
	{
		templateVector2 temp;
		temp.x = x - v.x;
		temp.y = y - v.y;

		return temp;
	}

	templateVector2 operator+=(const templateVector2& v)
	{
		x = x + v.x;
		y = y + v.y;

		return *this;
	}

	templateVector2 operator-=(const templateVector2& v)
	{
		x = x - v.x;
		y = y - v.y;

		return *this;
	}

	templateVector2 operator+=(const float& f)
	{
		x = x + f;
		y = y + f;

		return *this;
	}

	templateVector2 operator-=(const float& f)
	{
		x = x - f;
		y = y - f;

		return *this;
	}

	templateVector2 operator*=(const float& f)
	{
		x = x * f;
		y = y * f;

		return *this;
	}

	//de inteles bine
	//http://stackoverflow.com/questions/1243614/how-do-i-calculate-the-normal-vector-of-a-line-segment
	templateVector2 perpendicular()
	{
		templateVector2 temp;

		temp.x = -y;
		temp.y =  x;

		return temp;
	}

	templateVector2 normalize()
	{
		templateVector2 temp;
		float	length;
		temp.x = x;
		temp.y = y;		
		length = sqrt(x*x+y*y);
		temp.x = temp.x / length;
		temp.y = temp.y / length;

		return temp;
	}

	templateVector2 midpoint()
	{
		templateVector2 temp;
		temp.x = x/2;
		temp.y = y/2;

		return temp;
	}

	float length()
	{
		return sqrt(x*x+y*y);
	}

	T x;
	T y;
};

typedef templateVector2<float> Vector2f;

}

#endif