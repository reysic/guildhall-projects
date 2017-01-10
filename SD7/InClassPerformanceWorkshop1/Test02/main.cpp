#include "Map.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include "Engine/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>


//-----------------------------------------------------------------------------------------------
float ComputePathGCost( Map& theMap, const TileCoordsList& path )
{
	float totalCost = 0.f;
	for( int stepIndex = 0; stepIndex < (int) path.size(); ++ stepIndex )
	{
		const IntVector2& stepCoords = path[ stepIndex ];
		int tileIndex = theMap.GetTileIndexForCoords( stepCoords.x, stepCoords.y );
		TileType tileType = theMap.GetTileTypeAtCoords( stepCoords );
		float gCostHere = theMap.GetGCostForTileType( tileType );
		totalCost += gCostHere;
	}

	return totalCost;
}


//-----------------------------------------------------------------------------------------------
void TestPathingCorrectness( Map& theMap, int maxAttempts, float maxSeconds )
{
	int numTiles = theMap.m_width * theMap.m_height;
	TileCoordsList referencePath;
	TileCoordsList optimizedPath;
	referencePath.reserve( numTiles );
	optimizedPath.reserve( numTiles );

	double timeAtStart = GetCurrentTimeSeconds();
	double elapsedSeconds = 0.0;
	int numPathsTested = 0;
	int numPathsIncorrect = 0;
	while( numPathsTested < maxAttempts )
	{
		++ numPathsTested;
		elapsedSeconds = GetCurrentTimeSeconds() - timeAtStart;
		if( elapsedSeconds > maxSeconds )
			break;

		referencePath.clear();
		optimizedPath.clear();
		IntVector2 start = theMap.GetRandomInteriorNonSolidCoords();
		IntVector2 goal = theMap.GetRandomInteriorNonSolidCoords();

		// Compute the same start->goal path request two different ways (reference implementation and yours)
		theMap.ComputePath_Reference( start, goal, referencePath );
		theMap.ComputePath_Optimized( start, goal, optimizedPath );

		// Make sure both paths are essentially the same gCost; if not, one of them is producing an incorrect best path!
		float referencePathCost = ComputePathGCost( theMap, referencePath );
		float optimizedPathCost = ComputePathGCost( theMap, optimizedPath );
		float differenceInCosts = fabs( optimizedPathCost - referencePathCost );
		constexpr float EPSILON_TOLERANCE = 0.01f;
		if( differenceInCosts > EPSILON_TOLERANCE )
		{
			++ numPathsIncorrect;
			static bool hasPrintedMessageYet = false;
			if( !hasPrintedMessageYet )
			{
				printf( "\n*** PATH CALCULATION ERROR ***\n" );
				printf( "  Reference path had a cost of %.01f in %i steps.\n", referencePathCost, (int) referencePath.size() );
				printf( "  Optimized path had a cost of %.01f in %i steps.\n", optimizedPathCost, (int) optimizedPath.size() );

				printf( "  Correct (reference) path from (%i,%i) to (%i,%i) is shown below:\n", start.x, start.y, goal.x, goal.y );
				theMap.PrintMap( referencePath );

				printf( "  Incorrect (optimized) path from (%i,%i) to (%i,%i) is shown below:\n", start.x, start.y, goal.x, goal.y );
				theMap.PrintMap( optimizedPath );

				ERROR_RECOVERABLE( "Path calculation error!" );
				hasPrintedMessageYet = true;
			}

			if( g_debugVerbose )
				break;
		}
	}

	int numPathsCorrect = numPathsTested - numPathsIncorrect;
	if( numPathsIncorrect == 0 )
		printf( "\n*** PASSED *** path correctness test; all %i paths correct - took %.0f ms to verify.\n", numPathsTested, 1000.f * elapsedSeconds );
	else
		printf( "\n*** FAILED *** path correctness test: %i of %i paths correct (%i incorrect) - took %.0f ms.\n",
			numPathsCorrect, numPathsTested, numPathsIncorrect, 1000.f * elapsedSeconds );
}


//-----------------------------------------------------------------------------------------------
void TestPathingPerformance( Map& theMap, int maxAttempts, float maxSeconds )
{
	int numTiles = theMap.m_width * theMap.m_height;
	TileCoordsList path;
	TileCoordsList mostExpensivePath;
	path.reserve( numTiles );

	int numAttempts = 0;
	int numSuccesses = 0;
	int numFailures = 0;
	int totalSuccessesLengths = 0;
	int longestSuccessLength = 0;
	float totalSuccessesCosts = 0.0;
	float mostExpensivePathCost = 0.f;
	double totalAttemptsSeconds = 0.0;
	double totalSuccessesSeconds = 0.0;
	double totalFailuresSeconds = 0.0;
	double slowestSuccessSeconds = 0.0;
	double slowestFailureSeconds = 0.0;
	while( numAttempts < maxAttempts )
	{
		if( totalAttemptsSeconds > maxSeconds )
			break;

		path.clear();
		IntVector2 start = theMap.GetRandomInteriorNonSolidCoords();
		IntVector2 goal = theMap.GetRandomInteriorNonSolidCoords();

		// Compute the path and measure time
		double timeAtStart = GetCurrentTimeSeconds();
		theMap.ComputePath_Optimized( start, goal, path );
		double pathSeconds = GetCurrentTimeSeconds() - timeAtStart;

		// Record some quick metrics (not included in path profiling time)
		++ numAttempts;
		bool isPossible = !path.empty();
		totalAttemptsSeconds += pathSeconds;
		if( isPossible )
		{
			++ numSuccesses;
			int pathLength = (int) path.size();
			float pathCost = ComputePathGCost( theMap, path );
			totalSuccessesCosts += pathCost;
			totalSuccessesLengths += pathLength;
			totalSuccessesSeconds += pathSeconds;

			if( pathLength > longestSuccessLength )
				longestSuccessLength = pathLength;

			if( pathSeconds > slowestSuccessSeconds )
				slowestSuccessSeconds = pathSeconds;

			if( pathCost > mostExpensivePathCost )
			{
				mostExpensivePathCost = pathCost;
				mostExpensivePath.swap( path );
			}
		}
		else
		{
			++ numFailures;
			totalFailuresSeconds += pathSeconds;
			if( pathSeconds > slowestFailureSeconds )
				slowestFailureSeconds = pathSeconds;
		}
	}

	// Compute aggregate metrics
	float attemptsPerSecond = (float) numAttempts / (float) totalAttemptsSeconds;
	float successesPerSecond = (float) numSuccesses / (float) totalSuccessesSeconds;
	float failuresPerSecond = (float) numFailures / (float) totalFailuresSeconds;
	float avgMsPerAttempt = 1000.f / attemptsPerSecond;
	float avgMsPerSuccess = 1000.f / successesPerSecond;
	float avgMsPerFailure = 1000.f / failuresPerSecond;

	printf( "The most expensive (successful) path computed (gCost=%.1f) was:\n", mostExpensivePathCost );
	theMap.PrintMap( mostExpensivePath );

	printf( "\nComputed %i random paths in %.0f ms (%7.02f paths/sec, avg=%.03f ms/path):\n",
		numAttempts, 1000.f * totalAttemptsSeconds, attemptsPerSecond, avgMsPerAttempt );

	if( numSuccesses > 0 )
	{
		printf( "%6i successful paths (%7.02f paths/sec, avg=%6.03f ms, slowest=%6.03f ms, maxGCost=%.1f)\n",
			numSuccesses, successesPerSecond, avgMsPerSuccess, 1000.f * slowestSuccessSeconds, mostExpensivePathCost );
	}

	if( numFailures > 0 )
	{
		printf( "%6i failed non-paths (%7.02f fails/sec, avg=%6.03f ms, slowest=%6.03f ms)\n",
			numFailures, failuresPerSecond, avgMsPerFailure, 1000.f * slowestFailureSeconds );
	}

	printf( "\n" );
}


//-----------------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
	UNUSED( argc );
	UNUSED( argv );
//	srand( (unsigned int) time(nullptr) );

	Map theMap( 180, 32 ); // 13x9 for debugging, 70x20 for small map test, 180x32 medium, 180x180 large, 180x900 huge

	TestPathingCorrectness( theMap, 100, 3.f );
	TestPathingPerformance( theMap, 100000, 10.f );

	system( "pause" );
	return 0;
}

