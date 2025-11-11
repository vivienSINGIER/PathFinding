#ifndef GRID_CPP_INCLUDED
#define GRID_CPP_INCLUDED

#include "Grid.h"

#include "Utils.hpp"
#include "../Debug.h"
#include "../GridConfig.h"

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
    Init(gridConfigIndex);
}

void Grid::OnUpdate()
{
    Draw();
}

void Grid::OnEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        bool isSwapping = false;
        if (event.key.code == sf::Keyboard::F1)
        {
            gridConfigIndex--;
            isSwapping = true;
        }
        if (event.key.code == sf::Keyboard::F2)
        {
            gridConfigIndex++;
            isSwapping = true;
        }

        if (event.key.code == sf::Keyboard::B)
            ToggleWalkable();
        
        if (isSwapping)
        {
            m_pSelectedTile = nullptr;
            Clamp(gridConfigIndex, 1, 2);
            m_vData.clear();
            m_vNodes.clear();
            Init(gridConfigIndex);
        }
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            m_pSelectedTile = TrySelectedTile(event.mouseButton.x, event.mouseButton.y);
        }
    }
    
}

void Grid::Init(const int configIndex)
{
    std::vector<std::vector<char>> GRID = GridConfig::GetConfig(configIndex);
    int HEIGHT = GRID.size();
    int WIDTH = GRID[0].size();

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

    CalculateNodes();

    float offsetX = GetWindowWidth() / 2 - gridSize.x / 2;
    float offsetY = GetWindowHeight() / 2 - gridSize.y / 2;
    anchorPoint = sf::Vector2i(offsetX, offsetY);
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
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        Node<Tile>* n = &m_vNodes[i];
        Position p = n->data->position;

        sf::Color color = sf::Color::Green;
        if (n == m_pSelectedTile)
            color = sf::Color::Blue;
        if (n == m_pSelectedTile && n->data->isWalkable == false)
            color = sf::Color::Red;
        
        if (n->data->isWalkable || n == m_pSelectedTile)
            Debug::DrawFilledRectangle(anchorPoint.x + p.x * 50.f, anchorPoint.y + p.y * 50.f, 50.0f, 50.0f, color);
    }
}

void Grid::CalculateNodes()
{
    m_vNodes.clear();
    
    int WIDTH = m_vData[0].size();
    int HEIGHT = m_vData.size();

    gridSize = sf::Vector2i(WIDTH, HEIGHT) * 50;
    
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            Node<Tile> tempNode;
            tempNode.data = &m_vData[i][j];
            m_vNodes.push_back(tempNode);
        }
    }
    
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        Node<Tile>* n = &m_vNodes[i];
        n->vNeighbours.clear();
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

Node<Tile>* Grid::TrySelectedTile(int x, int y)
{
    sf::Vector2i pos = { x - anchorPoint.x, y - anchorPoint.y };
    if (pos.x < 0) pos.x -= 50;
    if (pos.y < 0) pos.y -= 50;
    pos /= 50;
    
    sf::Vector2i min = {m_vData[0][0].position.x - 1, m_vData[0][0].position.y - 1};
    sf::Vector2i max = {m_vData.back().back().position.x + 1, m_vData.back().back().position.y + 1};
    
    if (pos.x < min.x || pos.y < min.y) return nullptr;
    if (pos.x > max.x || pos.y > max.y) return nullptr;
    
    Node<Tile>* n = GetNode({pos.x, pos.y});

    if (n != nullptr) return n;
    
    AddTile({pos.x, pos.y});
    
    n = GetNode({pos.x, pos.y});

    if (n == nullptr)
        int i = 0;
    return n;
}

void Grid::AddTile(sf::Vector2i pos)
{
    // TODO Create nodes and recalculate neighbours
    
    if (pos.y < m_vData[0][0].position.y || pos.y > m_vData.back()[0].position.y)
    {
        std::vector<Tile> newVect;

        for (int i = 0; i < m_vData[0].size(); i++)
        {
            Tile t;
            t.position.x = m_vData[0][i].position.x;
            t.position.y = m_vData[0][0].position.y - 1;
            if (pos.y > m_vData.back()[0].position.y)
                t.position.y = m_vData.back()[0].position.y + 1;
            t.isWalkable = false;

            newVect.push_back(t);
        }

        if (pos.y < m_vData[0][0].position.y)
            m_vData.insert(m_vData.begin(), newVect);
        if (pos.y > m_vData.back()[0].position.y)
            m_vData.push_back(newVect);
    }
    else if (pos.x < m_vData[0][0].position.x)
    {
        int xPos = m_vData[0][0].position.x - 1;
        for (int i = 0; i < m_vData.size(); i++)
        {
            Tile t;
            t.position.x = xPos;
            t.position.y = m_vData[i][0].position.y;
            t.isWalkable = false;
            
            m_vData[i].insert(m_vData[i].begin(), t);
        }
    }
    else if (pos.x > m_vData[0].back().position.x)
    {
        int xPos = m_vData[0].back().position.x + 1;
        for (int i = 0; i < m_vData.size(); i++)
        {
            Tile t;
            t.position.x = xPos;
            t.position.y = m_vData[i][0].position.y;
            
            t.isWalkable = false;
            m_vData[i].push_back(t);
        }
    }

    CalculateNodes();
}

void Grid::ToggleWalkable()
{
    if (m_pSelectedTile == nullptr) return;

    m_pSelectedTile->data->isWalkable = !m_pSelectedTile->data->isWalkable;

    if (m_pSelectedTile->data->isWalkable == true)
    {
        for (int i = 0; i < m_pSelectedTile->vNeighbours.size(); i++)
        {
            m_pSelectedTile->vNeighbours[i]->vNeighbours.push_back(m_pSelectedTile);
        }
        return;
    }

    for (int i = 0; i < m_pSelectedTile->vNeighbours.size(); i++)
    {
        Node<Tile>* n = m_pSelectedTile->vNeighbours[i];
        for (int j = 0; j < n->vNeighbours.size(); j++)
        {
            if (n->vNeighbours[j] == m_pSelectedTile)
                n->vNeighbours.erase(n->vNeighbours.begin() + j);
        }
    }
}

#endif
