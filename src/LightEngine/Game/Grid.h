#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED
#include <vector>

#include "Node.hpp"
#include "Position.hpp"
#include "NodeData.hpp"
#include "../Scene.h"

class GridConfiguration;

class Grid : public Scene
{
public:
    Node<Tile>* GetNode(Position const& pos);

    void OnInitialize() override;
    void OnUpdate() override;
    void OnEvent(const sf::Event& event) override;
    
    void Init(const int configIndex);
    void Reset();
    void Update();
    void Draw();
private:
    sf::Vector2i gridSize;
    sf::Vector2i gridCenter;
    sf::Vector2i anchorPoint;

    int gridConfigIndex = 1;
    
    std::vector<std::vector<Tile>> m_vData;
    std::vector<Node<Tile>> m_vNodes;

    Node<Tile>* m_pSelectedTile = nullptr;

    void CalculateNodes();
    
    Node<Tile>* TrySelectedTile(int x, int y);
    void AddTile(sf::Vector2i pos);
    void ToggleWalkable();
};

#endif
