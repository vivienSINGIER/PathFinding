#ifndef POSITION_HPP_INCLUDED
#define POSITION_HPP_INCLUDED
#include <SFML/System/Vector2.hpp>

struct Position
{
    int x = 0;
    int y = 0;

    bool operator==(Position& other)
    {
        return x == other.x && y == other.y;
    }
    bool operator==(Position const& other)
    {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(Position& other)
    {
        return x != other.x || y != other.y;
    }
    bool operator!=(Position const& other)
    {
        return x != other.x || y != other.y;
    }
    
    Position& operator=(Position other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
    Position& operator+=(Position& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    static std::vector<Position> GetNeighbours(Position const& pos, sf::Vector2i maxSize, sf::Vector2i minSize)
    {
        if (pos.x < 0 || pos.y < 0)
            int o = 0;
        
        std::vector<Position> neighbours;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (j == 0 && i == 0)                 continue;
                if (pos.y + j < minSize.y || pos.x + i < minSize.x)                   continue;
                if (pos.y + j > maxSize.y || pos.x + i > maxSize.x)   continue;
                
                neighbours.push_back(Position{ pos.x + i, pos.y + j });
            }
        }
        return neighbours;
    }
};

#endif