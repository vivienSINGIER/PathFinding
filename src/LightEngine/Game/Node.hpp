#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include <map>
#include <vector>

template<typename T>
struct Node
{
    T* data;
    
    bool isVisited = false;
    Node* pCameFrom = nullptr;
    
    int cost = -1;
    int targetDistance = -1;

    std::vector<Node*> vNeighbours;

    bool operator()(Node<T> const* a, Node<T> const* b) const
    {
        int totalCostA = a->cost + a->targetDistance;
        int totalCostB = b->cost + b->targetDistance;

        return totalCostA > totalCostB;
    }
};

#endif