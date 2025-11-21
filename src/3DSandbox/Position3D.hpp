#ifndef POSITION3D_HPP_INCLUDED
#define POSITION3D_HPP_INCLUDED
#include <GC-simple-render/Geometries.h>

struct Position
{
    int x = 0;
    int y = 0;
	float height = 0.0f;

    bool operator==(Position& other)
    {
        return x == other.x && y == other.y && height == other.height;
    }
    bool operator==(Position const& other)
    {
        return x == other.x && y == other.y && height == other.height;
    }

    bool operator!=(Position& other)
    {
        return x != other.x || y != other.y || height != other.height;
    }
    bool operator!=(Position const& other)
    {
        return x != other.x || y != other.y || height != other.height;
    }

    Position& operator=(Position other)
    {
        x = other.x;
        y = other.y;
		height = other.height;
        return *this;
    }
    Position& operator+=(Position& other)
    {
        x += other.x;
        y += other.y;
		height += other.height;
        return *this;
    }

    static std::vector<Position> GetNeighbours(Position const& pos, gce::Vector2i32 maxSize, gce::Vector2i32 minSize)
    {
        std::vector<Position> neighbours;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (j == 0 && i == 0)                               continue;
                if (pos.y + j < minSize.y || pos.x + i < minSize.x) continue;
                if (pos.y + j > maxSize.y || pos.x + i > maxSize.x) continue;


                neighbours.push_back(Position{ pos.x + i, pos.y + j });
            }
        }
        return neighbours;
    }
};

#endif