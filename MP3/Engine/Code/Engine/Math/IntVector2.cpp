#include "Engine/Math/IntVector2.hpp"

IntVector2::IntVector2()
{
	x = 0;
	y = 0;
}

IntVector2::IntVector2(int initialX, int initialY)
{
	x = initialX;
	y = initialY;
}
IntVector2::IntVector2(const IntVector2& v) : x(v.x), y(v.y)
{

}

void IntVector2::SetXY(int newX, int newY)
{
	x = newX;
	y = newY;
}

float IntVector2::GetMagnitude()
{
	float xf = (float)x;
	float yf = (float)y;
	return (float)sqrt(xf*xf + yf*yf);
}

IntVector2& IntVector2::operator=(const IntVector2 &rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

const IntVector2 IntVector2::operator+(const IntVector2 other) const
{
	IntVector2 result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	return result;
}

IntVector2& IntVector2::operator+=(const IntVector2 other)
{
	this->x = x + other.x;
	this->y = y + other.y;
	return *this;
}

const IntVector2 IntVector2::operator-(const IntVector2 other) const
{
	IntVector2 result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	return result;
}

IntVector2 IntVector2::operator-=(const IntVector2 other)
{
	this->x = x - other.x;
	this->y = y - other.y;
	return *this;
}

IntVector2 IntVector2::operator*(int scalar)
{
	IntVector2 result;
	result.x = x * scalar;
	result.y = y * scalar;
	return result;
}

const IntVector2 IntVector2::operator*(const int& scalar) const
{
	IntVector2 result;
	result.x = x * scalar;
	result.y = y * scalar;
	return result;
}

bool IntVector2::operator<(const IntVector2& rhs)
{
	if (y < rhs.y) 
		return true;
	if (rhs.y < y) 
		return false;
	return x < rhs.x;

}

bool operator<(const IntVector2& lhs, const IntVector2& rhs)
{
	if (lhs.y < rhs.y)
		return true;
	if (rhs.y < lhs.y)
		return false;
	return lhs.x < rhs.x;
}

IntVector2 operator*(int scalar, const IntVector2& vec)
{
	IntVector2 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	return result;
}


bool operator==(const IntVector2& lhs, const IntVector2& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y)
		return true;
	return false;
}

bool operator!=(const IntVector2& lhs, const IntVector2& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y)
		return false;
	return true;
}