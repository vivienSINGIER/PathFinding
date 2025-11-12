#pragma once

#include <vector>

class GridConfig
{
	static std::vector<std::vector<char>> config1;

	static std::vector<std::vector<char>> config2;

public:
	static std::vector<std::vector<char>>& GetConfig(int configIndex);
};

