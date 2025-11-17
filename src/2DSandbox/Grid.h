#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED
#include <vector>

#include "Node.hpp"
#include "Position.hpp"
#include "NodeData.hpp"
#include "Scene.h"

struct Tile;
class GridConfiguration;
class Agent;

class Grid : public Scene
{
public:
    Node<Tile>* GetNode(Position const& pos);
    Node<Tile>* GetNode(sf::Vector2i const& pos);
    static sf::Vector2i GetTilePosition(sf::Vector2i worldPos);
    static sf::Vector2i GetWorldPosition(sf::Vector2i gridPos);

    void OnInitialize() override;
    void OnUpdate() override;
    void OnEvent(const sf::Event& event) override;
    
    void Init(const int configIndex);
    void Reset();
    void Draw();

    void CreateAgent(sf::Vector2i mousePos);

    Node<Tile>* AStar(Node<Tile>* startNode, Node<Tile>* endNode, Agent* pAgent);

private:
    sf::Vector2i m_gridSize;
    sf::Vector2i m_gridCenter;
    static sf::Vector2i m_anchorPoint;            

    int m_gridConfigIndex = 1;
    
    std::vector<std::vector<Tile>> m_vData;
    std::vector<Node<Tile>> m_vNodes;
    std::vector<Agent*> m_vAgents;

    Node<Tile>* m_pSelectedTile = nullptr;
    Agent* m_pSelectedAgent = nullptr;

    void CalculateNodes();
    
    Node<Tile>* TrySelectedTile(int x, int y);
    std::vector<Node<Tile>*> GetTouchingTiles(Agent* pAgent);
    void TrySelectedAgent(int x, int y);
    void AddTile(sf::Vector2i pos);
    void ToggleWalkable();

    void SaveMap();
};

#endif
