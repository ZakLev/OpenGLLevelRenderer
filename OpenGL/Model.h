#pragma once
#pragma once
#include <vector>
#include <string>
#include "../Gateware/Gateware.h"
#define GATEWARE_ENABLE_MATH  // Math Library
#include "h2bParser.h"



class Model // Object Oriented not Data Oriented -- except they are in seperate vectors instead of a model object
{
private:

public:

	GW::MATH::GMATRIXF worldPosition = {};
	std::string Name = {};
	std::string NamePath = {};
	H2B::Parser parser = {};
	/*GW::MATH::GMATRIXF Model::GetWorldPosition() { return worldPosition; };
	void Model::SetWorldPosition( GW::MATH::GMATRIXF& wp) { worldPosition = wp; };

	std::vector<std::string> Model::GetNames() { return Names; };
	void Model::SetNames(std::vector < std::string >& N) { Names = N; };

	std::vector<H2B::Parser> Model::GetParser() { return parsers; };
	void Model::SetParser(std::vector< H2B::Parser >& _parsers) { parsers = _parsers; };*/
};
