#ifndef GRID3D_CPP_INCLUDED
#define GRID3D_CPP_INCLUDED

#include "Grid3D.h"

#include <fstream>
#include <queue>

#include "../3DLightEngine/Utils.h"
#include "../3DLightEngine/Debug.h"
#include "GridConfig3D.h"
#include "Agent3D.h"

gce::Vector3f32 Grid3D::m_anchorPoint = { 0, 0, 0 };

Node<Tile>* Grid3D::GetNode(Position const& pos)
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

Node<Tile>* Grid3D::GetNode(gce::Vector2i8 const& pos)
{
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        if (m_vNodes[i].data->position.x == pos.x && m_vNodes[i].data->position.y == pos.y)
        {
            return &m_vNodes[i];
        }
    }
    return nullptr;
}

void Grid3D::OnInitialize()
{
    Init(m_gridConfigIndex);
}

void Grid3D::OnUpdate()
{
    Draw();
    Reset();

    for (int i = 0; i < m_vNodes.size(); i++)
    {
        m_vNodes[i].data->pOccupyingAgent = nullptr;
    }

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        Node<Tile>* n = GetNode(m_vAgents[i]->GetTilePosition());
        if (n != nullptr)
            n->data->pOccupyingAgent = m_vAgents[i];

        std::vector<Node<Tile>*> v = GetTouchingTiles(m_vAgents[i]);
        for (int j = 0; j < v.size(); j++)
        {
            v[j]->data->pOccupyingAgent = m_vAgents[i];
        }
    }

    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
    //{
    //    if (m_pSelectedAgent != nullptr)
    //    {
    //        sf::Window* w = GameManager::Get()->GetWindow();
    //        m_pSelectedAgent->PreviewPath(GetTilePosition(sf::Mouse::getPosition(*w)));
    //    }
    //}

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        if (m_vAgents[i]->ToDestroy() == true)
        {
            m_vAgents.erase(m_vAgents.begin() + i);
        }
    }

}

//void Grid3D::OnEvent(const sf::Event& event)
//{
//    if (event.type == sf::Event::KeyPressed)
//    {
//        bool isSwapping = false;
//        if (event.key.code == sf::Keyboard::F1)
//        {
//            m_gridConfigIndex--;
//            isSwapping = true;
//        }
//        if (event.key.code == sf::Keyboard::F2)
//        {
//            m_gridConfigIndex++;
//            isSwapping = true;
//        }
//
//        if (event.key.code == sf::Keyboard::B)
//            ToggleWalkable();
//
//        if (event.key.code == sf::Keyboard::Delete)
//        {
//            if (m_pSelectedAgent != nullptr)
//            {
//                m_pSelectedAgent->Destroy();
//                m_pSelectedAgent = nullptr;
//            }
//        }
//
//        if (event.key.code == sf::Keyboard::P)
//        {
//            if (m_pSelectedAgent != nullptr)
//            {
//                m_pSelectedAgent->ToggleLoop();
//            }
//        }
//
//        if (event.key.code == sf::Keyboard::Add)
//        {
//            m_pSelectedAgent->SetSpeedFactor(std::min(5.f, m_pSelectedAgent->GetSpeedFactor() + 1));
//        }
//
//        if (event.key.code == sf::Keyboard::Subtract)
//        {
//            m_pSelectedAgent->SetSpeedFactor(std::max(0.f, m_pSelectedAgent->GetSpeedFactor() - 1));
//        }
//
//        if (event.key.code == sf::Keyboard::S && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
//            SaveMap();
//
//        if (isSwapping)
//        {
//            m_pSelectedTile = nullptr;
//            Clamp(m_gridConfigIndex, 1, 2);
//            m_vData.clear();
//            m_vNodes.clear();
//            Init(m_gridConfigIndex);
//        }
//    }
//
//    if (event.type == sf::Event::MouseButtonPressed)
//    {
//        if (event.mouseButton.button == sf::Mouse::Middle)
//        {
//            m_pSelectedTile = TrySelectedTile(event.mouseButton.x, event.mouseButton.y);
//        }
//        if (event.mouseButton.button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::A))
//        {
//            CreateAgent({ event.mouseButton.x, event.mouseButton.y });
//        }
//        if (event.mouseButton.button == sf::Mouse::Left)
//        {
//            TrySelectedAgent(event.mouseButton.x, event.mouseButton.y);
//        }
//        if (event.mouseButton.button == sf::Mouse::Right)
//        {
//            if (m_pSelectedAgent != nullptr)
//            {
//                Node<Tile>* target = GetNode(GetTilePosition({ event.mouseButton.x, event.mouseButton.y }));
//                if (target != nullptr && target->data->isWalkable == true)
//                {
//                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) == false)
//                        m_pSelectedAgent->ResetPaths();
//                    m_pSelectedAgent->AddPath(GetTilePosition({ event.mouseButton.x, event.mouseButton.y }));
//                    Reset();
//                }
//            }
//        }
//
//    }
//}

void Grid3D::Init(const int configIndex)
{
    GridConfig3D::LoadConfigs();

    std::vector<std::vector<char>> GRID = GridConfig3D::GetConfig(configIndex);
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
            if (GRID[i][j] == '0')
                tempTile.isWalkable = false;
            else
                tempTile.isWalkable = true;
            tempTiles.push_back(tempTile);
        }
        m_vData.push_back(tempTiles);
    }

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        m_vAgents[i]->Destroy();
    }
    m_pSelectedAgent = nullptr;

    CalculateNodes();

    float offsetX = GetWindowWidth() / 2 - m_gridSize.x / 2;
    float offsetY = GetWindowHeight() / 2 - m_gridSize.y / 2;
    Grid3D::m_anchorPoint = gce::Vector3f32(offsetX, 0.0f, offsetY);
}

void Grid3D::Reset()
{
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        m_vNodes[i].cost = MAX_COST;
        m_vNodes[i].targetDistance = MAX_COST;
        m_vNodes[i].pCameFrom = nullptr;
        m_vNodes[i].isVisited = false;
    }
}

void Grid3D::Draw()
{
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        Node<Tile>* n = &m_vNodes[i];
        Position p = n->data->position;

        gce::Color color = gce::Color::Green;
        if (n->data->pOccupyingAgent != nullptr)
            color = gce::Color::White;

        if (n == m_pSelectedTile)
            color = gce::Color::Blue;
        if (n == m_pSelectedTile && n->data->isWalkable == false)
            color = gce::Color::Red;

    //    if (n->data->isWalkable || n == m_pSelectedTile)
    //        Debug::DrawFilledRectangle(m_anchorPoint.x + p.x * 50.f, m_anchorPoint.y + p.y * 50.f, 50.0f, 50.0f, color);
    }

    if (m_pSelectedAgent != nullptr)
    {
        gce::Vector3f32 position = m_pSelectedAgent->GetPosition();
        /*Debug::DrawCircle(position.x, position.y, 10, sf::Color::Red);*/
    }

    for (Agent3D* agent : m_vAgents)
        agent->DrawPaths();
}

//void Grid3D::CreateAgent(gce::Vector2i8 mousePos)
//{
//    sf::Vector2i pos = GetTilePosition(mousePos);
//
//    sf::Vector2i min = { m_vData[0][0].position.x, m_vData[0][0].position.y };
//    sf::Vector2i max = { m_vData.back().back().position.x, m_vData.back().back().position.y };
//
//    if (pos.x < min.x || pos.y < min.y) return;
//    if (pos.x > max.x || pos.y > max.y) return;
//
//    Node<Tile>* tempNode = GetNode(pos);
//    if (tempNode == nullptr)
//        return;
//    if (tempNode->data->isWalkable == false || tempNode->data->pOccupyingAgent != nullptr)
//        return;
//
//    pos *= 50;
//
//    Agent* tempAgent = CreateEntity<Agent>(20.f, sf::Color::Blue);
//    tempAgent->SetPosition(pos.x + 25.f + m_anchorPoint.x, pos.y + 25.f + m_anchorPoint.y);
//
//    m_vAgents.push_back(tempAgent);
//}

Node<Tile>* Grid3D::AStar(Node<Tile>* startNode, Node<Tile>* endNode, Agent3D* pAgent)
{
    std::priority_queue<Node<Tile>*, std::vector<Node<Tile>*>, Node<Tile>> queue;
    startNode->cost = 0;
    queue.push(startNode);

    while (!queue.empty())
    {
        Node<Tile>* curr = queue.top();
        queue.pop();

        if (curr->isVisited == true)
            continue;

        if (curr == endNode)
            return endNode;

        curr->isVisited = true;

        std::vector<Node<Tile>*> neighbours = curr->vNeighbours;
        for (int i = 0; i < neighbours.size(); i++)
        {
            Node<Tile>* neighbour = neighbours[i];
            if (neighbour->isVisited)
                continue;
            if (neighbour->data->pOccupyingAgent != nullptr && neighbour->data->pOccupyingAgent != pAgent)
                continue;

            neighbour->targetDistance = neighbour->data->Distance(endNode->data);
            int newCost = curr->cost + curr->data->Distance(neighbour->data);
            if (newCost < neighbour->cost)
            {
                neighbour->cost = newCost;
                neighbour->pCameFrom = curr;
            }
            queue.push(neighbour);
        }
    }

    return nullptr;
}

gce::Vector2i8 Grid3D::GetTilePosition(gce::Vector3f32 worldPos)
{
    gce::Vector2i8 pos = { (int8)(worldPos.x - m_anchorPoint.x), (int8)(worldPos.y - m_anchorPoint.y) };
    if (pos.x < 0) pos.x -= 50;
    if (pos.y < 0) pos.y -= 50;
    pos /= 50;

    return pos;
}

gce::Vector3f32 Grid3D::GetWorldPosition(gce::Vector2i8 gridPos)
{
    return { m_anchorPoint.x + gridPos.x * 50 + 25, m_anchorPoint.y + gridPos.y * 50 + 25, m_anchorPoint.z * 50 + 25 };
}

void Grid3D::CalculateNodes()
{
    m_vNodes.clear();

    int WIDTH = m_vData[0].size();
    int HEIGHT = m_vData.size();

    m_gridSize = gce::Vector2i8(WIDTH, HEIGHT) ;

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
        std::vector<Position> neighbours = Position::GetNeighbours(n->data->position, { (int8)(m_vData[0].size() - 1), (int8)(m_vData.size() - 1) });

        for (int i = 0; i < neighbours.size(); i++)
        {
            Position p = neighbours[i];
            Tile* neig = &m_vData[p.y][p.x];

            if (neig->isWalkable == false)
                continue;
            if (n->data->Distance(neig) == 2)
            {
                Position centerPos = n->data->position;
                Tile* n1 = &m_vData[p.y][centerPos.x];
                Tile* n2 = &m_vData[centerPos.y][p.x];

                if (n1->isWalkable == false || n2->isWalkable == false)
                    continue;
            }

            n->vNeighbours.push_back(GetNode(neig->position));
        }
    }
}

//Node<Tile>* Grid3D::TrySelectedTile(int x, int y)
//{
//    gce::Vector2i8 pos = GetTilePosition({ x, y });
//
//    gce::Vector2i8 min = { m_vData[0][0].position.x - 1, m_vData[0][0].position.y - 1 };
//    gce::Vector2i8 max = { m_vData.back().back().position.x + 1, m_vData.back().back().position.y + 1 };
//
//    if (pos.x < min.x || pos.y < min.y) return nullptr;
//    if (pos.x > max.x || pos.y > max.y) return nullptr;
//
//    Node<Tile>* n = GetNode(pos);
//
//    if (n != nullptr) return n;
//
//    AddTile({ pos.x, pos.y });
//
//    n = GetNode(pos);
//
//    if (n == nullptr)
//        int i = 0;
//    return n;
//}

std::vector<Node<Tile>*> Grid3D::GetTouchingTiles(Agent3D* pAgent)
{
    std::vector<gce::Vector3f32> directions;
    std::vector<Node<Tile>*> result;

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (j == 0 && i == 0) continue;
            directions.push_back({ (float32)i, 0.f, (float32)j });
        }
    }

    gce::Vector3f32 pos =
    {
        pAgent->GetPosition().x,
        pAgent->GetPosition().y,
        pAgent->GetPosition().z };
    Node<Tile>* pNode = GetNode(pAgent->GetTilePosition());
    if (pNode == nullptr) return result;

    for (int i = 0; i < directions.size(); i++)
    {
        gce::Vector3f32 offset = directions[i];
        offset.x *= pAgent->GetRadius();
        offset.z *= pAgent->GetRadius();
        gce::Vector3f32 newPos = pos + offset;

        Node<Tile>* newNode = GetNode(GetTilePosition(newPos));
        if (newNode == nullptr) continue;
        if (newNode == pNode)   continue;

        result.push_back(newNode);
    }

    if (result.size() > 0)
        int o = 0;

    return result;
}

//void Grid3D::TrySelectedAgent(int x, int y)
//{
//    for (Agent* agent : m_vAgents)
//    {
//        if (agent->IsInside(x, y) == false)
//            continue;
//
//        m_pSelectedAgent = agent;
//    }
//}

void Grid3D::AddTile(gce::Vector2i8 pos)
{
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

void Grid3D::ToggleWalkable()
{
    if (m_pSelectedTile == nullptr) return;
    if (m_pSelectedTile->data->pOccupyingAgent != nullptr) return;

    m_pSelectedTile->data->isWalkable = !m_pSelectedTile->data->isWalkable;

    for (int i = 0; i < m_pSelectedTile->vNeighbours.size(); i++)
    {
        Node<Tile>* n = m_pSelectedTile->vNeighbours[i];
        n->vNeighbours.clear();
        std::vector<Position> neighbours = Position::GetNeighbours(n->data->position, { (int8)(m_vData[0].size() - 1), (int8)(m_vData.size() - 1) });

        for (int i = 0; i < neighbours.size(); i++)
        {
            Position p = neighbours[i];
            Tile* neig = &m_vData[p.y][p.x];

            if (neig->isWalkable == false)
                continue;
            if (n->data->Distance(neig) == 2)
            {
                Position centerPos = n->data->position;
                Tile* n1 = &m_vData[p.y][centerPos.x];
                Tile* n2 = &m_vData[centerPos.y][p.x];

                if (n1->isWalkable == false || n2->isWalkable == false)
                    continue;
            }

            n->vNeighbours.push_back(GetNode(neig->position));
        }
    }
}

void Grid3D::SaveMap()
{
    std::string filePath = "/../../res/Map";
    filePath.append(std::to_string(m_gridConfigIndex));
    filePath.append(".txt");

    std::ofstream file(filePath.c_str());
    if (!file.is_open()) return;

    std::string line;

    for (int i = 0; i < m_vData.size(); i++)
    {
        std::string temp = "";
        for (int j = 0; j < m_vData[i].size(); j++)
        {
            if (m_vData[i][j].isWalkable)
                temp.append("1");
            else
                temp.append("0");
        }
        temp.append("\n");
        file << temp;
    }

    file.close();
}

#endif
