#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

std::string ExtractToken(const std::string& inputString, const std::string& stringBeforeToken, const std::string& delimiter = "\n");




