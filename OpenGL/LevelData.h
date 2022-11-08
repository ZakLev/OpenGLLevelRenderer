#pragma once
#include <vector>
#include <string>
#include "../Gateware/Gateware.h"
namespace LevelData {

	class LevelData
	{
	private:
		std::vector<GW::MATH::GMATRIXF> worldPositions = {};
		std::vector<std::string> Names = {};
	public:
		std::vector<GW::MATH::GMATRIXF> LevelData::GetWorldPosition() { return worldPositions; };
		void LevelData::SetWorldPosition(std::vector < GW::MATH::GMATRIXF > wp) { worldPositions = wp; };

		std::vector<std::string> LevelData::GetNames() { return Names; };
		void LevelData::SetNames(std::vector < std::string > N) { Names = N; };
	};
}