#include "pch.h"
#ifndef GRID_CONFIG3D_CPP_INCLUDED
#define GRID_CONFIG3D_CPP_INCLUDED

#include "GridConfig3D.h"

#include <fstream>
#include <ios>
#include <string>

#include "Grid3D.h"

std::vector<std::vector<char>> GridConfig3D::config1;
std::vector<std::vector<char>> GridConfig3D::config2;
std::vector<std::vector<char>> GridConfig3D::config3;
std::vector<std::vector<char>> GridConfig3D::config4;

std::vector<std::vector<char>> GridConfig3D::LoadConfig(int index)
{
    std::vector<std::vector<char>> config;

    std::string filePath = "../../res/Map";
    filePath.append(std::to_string(index));
    filePath.append(".txt");

    std::fstream file(filePath.c_str(), std::ios::in);

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

    file.close();
    return config;

}

void GridConfig3D::LoadConfigs()
{
    GridConfig3D::config1 = LoadConfig(1);
    GridConfig3D::config2 = LoadConfig(2);
    GridConfig3D::config3 = LoadConfig(3);
    GridConfig3D::config4 = LoadConfig(4);
}

std::vector<std::vector<char>>& GridConfig3D::GetConfig(int configIndex)
{
    switch (configIndex)
    {
    case 1:
        return config1;
    case 2:
        return config2;
    case 3:
        return config3;
    case 4:
        return config4;
    default:
        break;
    }
}

#endif