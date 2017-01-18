#include "ShortVector2.hpp"


//-----------------------------------------------------------------------------------------------
ShortVector2::ShortVector2()
{
}


//-----------------------------------------------------------------------------------------------
ShortVector2::ShortVector2( short initialX, short initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
std::string ShortVector2::ToString()
{
	std::string output = "";

	output += std::to_string( x );
	output += ",";
	output += std::to_string( y );

	return output;
}


//-----------------------------------------------------------------------------------------------
bool ShortVector2::operator < ( const ShortVector2& rhs ) const
{
	if ( y < rhs.y )
		return true;
	if ( rhs.y < y )
		return false;
	return ( x < rhs.x );
}