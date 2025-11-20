#ifndef GRID3D_H_INCLUDED
#define GRID3D_H_INCLUDED

#include "3DLightEngine\Scene.h"

#include <vector>

#include "Node3D.hpp"
#include "Position3D.hpp"
#include "NodeData3D.hpp"

struct Tile;
class GridConfiguration;
class Agent3D;

class Grid3D : public Scene
{
public:
    Node<Tile>* GetNode(Position const& pos);
    Node<Tile>* GetNode(gce::Vector2i32 const& pos);
    static gce::Vector2i32 GetTilePosition(gce::Vector3f32 worldPos);
    static gce::Vector3f32 GetWorldPosition(gce::Vector2i32 gridPos);
    static gce::Vector3f32 GetWorldPosition(Position const& gridPos);

    void OnInitialize() override;
    void OnUpdate() override;
    void HandleInput() override;
    void CameraControl();
    //void OnEvent(const sf::Event& event) override;

    void Init(const int configIndex);
    void Reset();
    void Draw();

    void CreateAgent();

    Node<Tile>* AStar(Node<Tile>* startNode, Node<Tile>* endNode, Agent3D* pAgent);

private:
    gce::Vector2i32 m_gridSize;
    gce::Vector2i32 m_gridCenter;
    static gce::Vector3f32 m_anchorPoint;
	int m_ZoomLevel = 0;

    gce::Vector2i32 cursorPos;

    int m_gridConfigIndex = 1;

    std::vector<std::vector<Tile>> m_vData;
    std::vector<Node<Tile>> m_vNodes;
    std::vector<Agent3D*> m_vAgents;

    Node<Tile>* m_pSelectedTile = nullptr;
    Agent3D* m_pSelectedAgent = nullptr;
    int m_selectedAgentIndex = 0;

    void CalculateNodes();

    Node<Tile>* TrySelectedTile(int x, int y);
    std::vector<Node<Tile>*> GetTouchingTiles(Agent3D* pAgent);
    void AddTile(gce::Vector2i32 pos);
    void ToggleWalkable();
    void DeleteAgent();

    void SaveMap();
};

#endif
