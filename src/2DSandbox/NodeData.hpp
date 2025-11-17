#ifndef NODE_DATA_HPP_INCLUDED
#define NODE_DATA_HPP_INCLUDED

#include <cmath>

#include "Grid.h"
#include "Position.hpp"

class Agent;

struct NodeData
{
    virtual float Distance(NodeData const* other) { return 0.f; }
};

struct Tile : public NodeData
{
    Position position;
    bool isWalkable;
    Agent* pOccupyingAgent;
    
    float Distance(NodeData const* other) override
    {
        Tile const* otherTile = dynamic_cast<Tile const*>(other);
        if (otherTile == nullptr) return 0;
        
        return float(abs(position.x - otherTile->position.x) + abs(position.y - otherTile->position.y));
    }
};

#endif