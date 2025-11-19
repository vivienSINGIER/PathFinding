#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <map>
#include <vector>

#define MAX_COST (2147483647 / 2)

template<typename T>
struct Node
{
    T* data;
    
    bool isVisited = false;
    Node* pCameFrom = nullptr;
    
    int cost = MAX_COST;
    int targetDistance = MAX_COST;

    std::vector<Node*> vNeighbours;

    bool operator()(Node<T> const* a, Node<T> const* b) const
    {
        int totalCostA = a->cost + a->targetDistance;
        int totalCostB = b->cost + b->targetDistance;

        if ( totalCostA == totalCostB)
            return a->cost < b->cost;

        return totalCostA > totalCostB;
    }
};

#endif