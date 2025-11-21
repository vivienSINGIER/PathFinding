#ifndef GRID_CONFIG_H_INCLUDED
#define GRID_CONFIG_H_INCLUDED

#include <vector>

class GridConfig
{
	static std::vector<std::vector<char>> config1;
	static std::vector<std::vector<char>> config2;
	static std::vector<std::vector<char>> config3;
	static std::vector<std::vector<char>> config4;

	static std::vector<std::vector<char>> LoadConfig(int index);
	
public:
	static void LoadConfigs();
	static std::vector<std::vector<char>>& GetConfig(int configIndex);
};
#endif

