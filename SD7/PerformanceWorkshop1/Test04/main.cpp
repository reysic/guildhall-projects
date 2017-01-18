#include "Engine/ErrorWarningAssert.hpp"
#include "Engine/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void) (x)


//-----------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
	UNUSED( argc );
	UNUSED( argv );

	printf( "Hello, world.\n" );
	system( "pause" );
	return 0;
}

