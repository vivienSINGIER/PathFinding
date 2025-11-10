#ifndef GRID_CPP_INCLUDED
#define GRID_CPP_INCLUDED

#include "Grid.h"
#include "../Debug.h"
#include "../GridConfig.h"

#define WIDTH 10
#define HEIGHT 10

// std::vector<std::vector<char>> GRID =
// {
//     {' ', ' ', ' ', ' ', 'x', ' ', ' ', ' ', ' ', ' '},  
//     {' ', ' ', ' ', 'x', ' ', ' ', 'x', ' ', ' ', ' '},  
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x', ' ', ' '},  
//     {' ', 'x', 'x', 'x', ' ', ' ', 'x', ' ', 'x', ' '},  
//     {' ', 'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
//     {' ', 'x', ' ', ' ', ' ', ' ', ' ', 'x', ' ', 'x'},  
//     {' ', ' ', ' ', 'x', 'x', ' ', 'x', ' ', ' ', 'x'},  
//     {' ', ' ', ' ', 'x', ' ', ' ', ' ', ' ', ' ', 'x'},  
//     {' ', 'x', 'x', 'x', ' ', ' ', ' ', ' ', ' ', 'x'},  
//     {' ', ' ', ' ', ' ', ' ', ' ', 'x', ' ', ' ', 'x'},  
//     {' ', ' ', 'x', ' ', 'x', 'x', ' ', ' ', ' ', ' '}  
// };

Node<Tile>* Grid::GetNode(Position const& pos)
{
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        if (m_vNodes[i].data->position == pos)
        {
            return &m_vNodes[i];
        }
    }
    return nullptr;
}

void Grid::OnInitialize()
{
    Init(1);
}

void Grid::OnUpdate()
{
    Draw();
}

void Grid::OnEvent(const sf::Event& event)
{
}

void Grid::Init(const int configIndex)
{
    std::vector<std::vector<char>> GRID = GridConfig().GetConfig(configIndex);

    if (GRID.empty()) return;
    
    // Tiles & Nodes Vect
    for (int i = 0; i < HEIGHT; i++)
    {
        std::vector<Tile> tempTiles;
        for (int j = 0; j < WIDTH; j++)
        {
            Tile tempTile;
            tempTile.position.x = j;
            tempTile.position.y = i;
            if (GRID[i][j] == '#')
                tempTile.isWalkable = false;
            else
                tempTile.isWalkable = true;
            tempTiles.push_back(tempTile);
        }
        m_vData.push_back(tempTiles);
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            Node<Tile> tempNode;
            tempNode.data = &m_vData[i][j];
            m_vNodes.push_back(tempNode);
        }
    }

    // Node Neighbours
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        Node<Tile>* n = &m_vNodes[i];
        std::vector<Position> neighbours = Position::GetNeighbours(n->data->position, {(int)m_vData[0].size() - 1, (int)m_vData.size() - 1});
        
        for (int i = 0; i < neighbours.size(); i++)
        {
            Position p = neighbours[i];
            Tile* neig = &m_vData[p.y][p.x];
            if (neig->isWalkable == false)
                continue;
        
            n->vNeighbours.push_back(GetNode(neig->position));
        }
    }
}

void Grid::Reset()
{
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        m_vNodes[i].cost = -1;
        m_vNodes[i].targetDistance = -1;
        m_vNodes[i].pCameFrom = nullptr;
    }
}

void Grid::Update()
{
}

void Grid::Draw()
{
    for (Node<Tile> node : m_vNodes)
    {
        Position p = node.data->position;
        if (node.data->isWalkable)
            Debug::DrawRectangle(p.x * 50.f + 5.f, p.y * 50.f + 5.f, 50.0f, 50.0f, sf::Color::Green);
    }
}

#endif
