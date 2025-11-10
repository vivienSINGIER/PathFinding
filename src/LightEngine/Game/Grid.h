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

    Position cursorPos;
    Position playerPos;

    void OnInitialize() override;
    void OnUpdate() override;
    void OnEvent(const sf::Event& event) override;
    
    void Init(const int configIndex);
    void Reset();
    void Update();
    void Draw();
private:
    std::vector<std::vector<Tile>> m_vData;
    std::vector<Node<Tile>> m_vNodes;
    
};

#endif
