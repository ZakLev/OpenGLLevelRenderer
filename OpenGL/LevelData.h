#pragma once
#include <vector>
#include <string>
#include "../Gateware/Gateware.h"
#define GATEWARE_ENABLE_MATH  // Math Library
#include "h2bParser.h"



	class LevelData
	{
	private:
		
	public:
		
		std::vector<GW::MATH::GMATRIXF> worldPositions = {};
		std::vector<std::string> Names = {};
		std::vector<H2B::Parser> parsers = {};
		std::vector<GW::MATH::GMATRIXF> LevelData::GetWorldPosition() { return worldPositions; };
		void LevelData::SetWorldPosition(std::vector < GW::MATH::GMATRIXF >& wp) { worldPositions = wp; };

		std::vector<std::string> LevelData::GetNames() { return Names; };
		void LevelData::SetNames(std::vector < std::string >& N) { Names = N; };

		std::vector<H2B::Parser> LevelData::GetParser() { return parsers; };
		void LevelData::SetParser(std::vector< H2B::Parser >& _parsers) { parsers = _parsers; };
	};
