#pragma once

#include <map>
#include <string>

#include "Engine/Tools/Parsers/xmlParser.h"
#include "Game/Entities/Feature.hpp"


//-----------------------------------------------------------------------------------------------
class FeatureFactory
{
public:
	FeatureFactory( const XMLNode& featureBlueprintNode );
	static void LoadFeatureBlueprints();
	Feature* Create() { return new Feature( *m_templateFeature ); };
	static Feature* CreateByName( const std::string& factoryName, FeatureType type );
	static std::vector<Feature*> MakeItemsOfType( FeatureType type, int amount = 1 );
	static std::map<std::string, FeatureFactory*>* s_featureFactories[ NUM_FEATURE_TYPES ];
private:
	void PopulateFromXMLNode( XMLNode featureBlueprintNode );
	Feature* m_templateFeature;
	std::string m_name;
};
