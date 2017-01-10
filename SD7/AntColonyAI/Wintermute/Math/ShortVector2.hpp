#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class ShortVector2
{
public:
	ShortVector2();
	ShortVector2( short initialX, short initialY );

	std::string ToString();

	bool ShortVector2::operator < ( const ShortVector2& rhs ) const;
	inline const ShortVector2 ShortVector2::operator + ( const ShortVector2& vect ) const { return ShortVector2( x + vect.x, y + vect.y ); }

	inline bool ShortVector2::operator == ( const ShortVector2& vect ) const { return ( x == vect.x ) && ( y == vect.y ); }
	inline bool ShortVector2::operator != ( const ShortVector2& vect ) const { return ( x != vect.x ) || ( y != vect.y ); }

public:
	short x;
	short y;
};