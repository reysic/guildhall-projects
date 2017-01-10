#include "Game/Entities/FeatureFactory.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
std::map< std::string, FeatureFactory* >* FeatureFactory::s_featureFactories[ NUM_FEATURE_TYPES ];


//-----------------------------------------------------------------------------------------------
void FeatureFactory::PopulateFromXMLNode( XMLNode featureBlueprintNode )
{
	m_templateFeature = new Feature( featureBlueprintNode, GameEntityType::GAME_ENTITY_FEATURE );
}


//-----------------------------------------------------------------------------------------------
FeatureFactory::FeatureFactory( const XMLNode& featureBlueprintNode )
	:m_name( "" )
{

	if ( featureBlueprintNode.getAttribute( "name" ) )
	{
		std::string nameStr = featureBlueprintNode.getAttribute( "name" );
		m_name = nameStr;

		PopulateFromXMLNode( featureBlueprintNode );
	}
}


//-----------------------------------------------------------------------------------------------
void FeatureFactory::LoadFeatureBlueprints()
{
	if ( s_featureFactories[ 0 ] == nullptr )
	{
		for ( int i = 0; i < NUM_FEATURE_TYPES; i++ )
		{
			s_featureFactories[ i ] = new std::map<std::string, FeatureFactory*>;
		}
	}

	std::vector< std::string > featureFiles = EnumerateFilesInFolder( "Data/Features",
		"*.Feature.xml" );

	for ( std::string str : featureFiles )
	{
		XMLNode node = XMLNode::openFileHelper( str.c_str() );
		XMLNode rootNode = node.getChildNode( 0 ); // Feature blueprints

		if ( !rootNode.IsContentEmpty() )
		{
			int npcCount = rootNode.nChildNode(); // # of FeatureBlueprint nodes

			for ( int i = 0; i < npcCount; i++ )
			{
				XMLNode thisNode = rootNode.getChildNode( i );
				std::string name = thisNode.getName();
				if ( name != "FeatureBlueprint" )
				{
					// Not a valid node, skip
					continue;
				}
				FeatureType nodeType = NUM_FEATURE_TYPES;
				if ( thisNode.getAttribute( "type" ) )
				{
					nodeType = Feature::GetFeatureTypeFromString( thisNode.getAttribute( "type" ) );
				}
				else
				{
					continue;
				}

				if ( nodeType == NUM_FEATURE_TYPES )
				{
					continue;
				}
				FeatureFactory* newFactory = new FeatureFactory( thisNode );
				if ( newFactory != nullptr && newFactory->m_name != "" )
				{
					s_featureFactories[ nodeType ]->insert( std::pair< std::string, FeatureFactory* >( newFactory->m_name, newFactory ) );
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Feature* FeatureFactory::CreateByName( const std::string& factoryName, FeatureType type )
{
	// Call Create()
	Feature* feature = nullptr;
	FeatureFactory* featureFactory = nullptr;

	ASSERT_OR_DIE( s_featureFactories != nullptr, "No Feature Factories." );

	std::map<std::string, FeatureFactory*>::iterator generatorRegistrationIter = s_featureFactories[ type ]->find( factoryName );

	if ( generatorRegistrationIter != s_featureFactories[ type ]->end() )
	{
		featureFactory = generatorRegistrationIter->second;
		feature = featureFactory->Create();
		return feature;
	}
	else
	{
		DebuggerPrintf( "Did not find Factory of name %s\n", factoryName );
	}
	return nullptr;
}



//-----------------------------------------------------------------------------------------------
std::vector< Feature* > FeatureFactory::MakeItemsOfType( FeatureType type, int amount /*= 1*/ )
{
	// Makes specified amount of items of a particular type. Picks randomly from the array.
	std::vector< Feature* > outputVector;

	int featureCount = 0;
	while ( featureCount < amount )
	{
		int featureFactoryIndex = RandomInt( s_featureFactories[ type ]->size(), 0 );

		auto featureFactoryIter = s_featureFactories[ type ]->begin();
		std::advance( featureFactoryIter, featureFactoryIndex );
		Feature* feature = ( featureFactoryIter )->second->Create();
		outputVector.push_back( feature );
		featureCount++;
	}

	return outputVector;
}