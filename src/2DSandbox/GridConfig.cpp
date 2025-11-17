#ifndef GRID_CONFIG_CPP_INCLUDED
#define GRID_CONFIG_CPP_INCLUDED

#include "GridConfig.h"

#include <fstream>
#include <ios>
#include <string>

#include "Grid.h"

std::vector<std::vector<char>> GridConfig::config1;
std::vector<std::vector<char>> GridConfig::config2;

std::vector<std::vector<char>> GridConfig::LoadConfig(int index)
{
	std::vector<std::vector<char>> config;

	std::string filePath = "../../../res/Map";
	filePath.append(std::to_string(index));
	filePath.append(".txt");

	std::fstream file(filePath.c_str(), std::ios::app);
	
	if (!file.is_open()) return config;
	
	std::string line;
	while (getline(file, line))
	{
		std::vector<char> vLine;
		for (int i = 0; i < line.size(); i++)
		{
			vLine.push_back(line[i]);
		}
		config.push_back(vLine);
	}

	return config;
}

void GridConfig::LoadConfigs()
{
	GridConfig::config1 = LoadConfig(1);
	GridConfig::config2 = LoadConfig(2);
}

std::vector<std::vector<char>>& GridConfig::GetConfig(int configIndex)
{
	switch (configIndex)
	{
	case 1:
		return config1;
	case 2:
		return config2;
	default:
		break;
	}
}

#endif