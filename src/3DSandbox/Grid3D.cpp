#include "pch.h"
#ifndef GRID3D_CPP_INCLUDED
#define GRID3D_CPP_INCLUDED

#include "Grid3D.h"

#include <fstream>
#include <queue>

#include "3DLightEngine/Utils.h"
#include "3DLightEngine/Debug.h"
#include "GridConfig3D.h"
#include "Agent3D.h"
#include "GC-simple-render/InputsMethods.h"

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

Node<Tile>* Grid3D::GetNode(gce::Vector2i32 const& pos)
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

    m_pSelectedTile = TrySelectedTile(cursorPos.x, cursorPos.y);
    if (m_vAgents.empty() == false)
    {
        Agent3D* newSelected = m_vAgents[m_selectedAgentIndex];
        if (newSelected != m_pSelectedAgent)
        {
            if (m_pSelectedAgent != nullptr)
                m_pSelectedAgent->SetColor({0.0f, 0.0f, 1.0f});
            m_pSelectedAgent = newSelected;
            m_pSelectedAgent->SetColor({1.0f, 1.0f, 0.0f});
        }
    }
    
    for (int i = 0; i < m_vNodes.size(); i++)
    {
        m_vNodes[i].data->pOccupyingAgent = nullptr;
    }

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        Node<Tile>* n = GetNode(m_vAgents[i]->GetTilePosition());
        if (n != nullptr && n->data->pOccupyingAgent == nullptr)
            n->data->pOccupyingAgent = m_vAgents[i];
    }

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        Node<Tile>* n = GetNode(m_vAgents[i]->GetTilePosition());
        if (n == nullptr) continue;
        std::vector<Node<Tile>*> v = GetTouchingTiles(m_vAgents[i]);
        if (v.size() > 0)
        {
            float32 height = n->data->position.height;
            for (int j = 0; j < v.size(); j++)
            {
                if (v[j]->data->pOccupyingAgent == nullptr)
                    v[j]->data->pOccupyingAgent = m_vAgents[i];
                if (v[j]->data->position.height > height)
                    height = v[j]->data->position.height;
            }
            m_vAgents[i]->UpdateHeight(height);   
        }
    }

    for (int i = 0; i < m_vAgents.size(); i++)
    {
        if (m_vAgents[i]->ToDestroy() == true)
        {
            m_vAgents.erase(m_vAgents.begin() + i);
        }
    }

}   

void Grid3D::HandleInput()
{
    // Select Grid Config
    int oldIndex = m_gridConfigIndex;
    if (GetKeyDown(Keyboard::F1))
        m_gridConfigIndex--;
    if (GetKeyDown(Keyboard::F2))
        m_gridConfigIndex++;
    m_gridConfigIndex = gce::Clamp(m_gridConfigIndex, 1, 4);
    if (m_gridConfigIndex != oldIndex)
        Init(m_gridConfigIndex);

    // Save Grid Config
    if (GetKeyDown(Keyboard::S))
    {
        if(GetKey(Keyboard::LCONTROL))
            SaveMap();
    }
    
    // Select tile
    if (GetKeyDown(Keyboard::LEFT))
        cursorPos.x -= 1;
    if (GetKeyDown(Keyboard::RIGHT))
        cursorPos.x += 1;
    if (GetKeyDown(Keyboard::UP))
        cursorPos.y -= 1;
    if (GetKeyDown(Keyboard::DOWN))
        cursorPos.y += 1;

    // Select Agent
    if (GetKeyDown(Keyboard::PAGE_UP) || GetKeyDown(Keyboard::I))
        m_selectedAgentIndex += 1;
    if (GetKeyDown(Keyboard::PAGE_DOWN) || GetKeyDown(Keyboard::U))
        m_selectedAgentIndex -= 1;
    if (m_vAgents.empty() == false)
        m_selectedAgentIndex %= m_vAgents.size();   

    // Add Agent
    if (GetKeyDown(Keyboard::A))
        CreateAgent();

    // Build / Delete blockk
    if (GetKeyDown(Keyboard::B))
        ToggleWalkable();

    CameraControl();
        
    // Create Path
    if (GetKeyDown(Keyboard::ENTER))
    {
        if (m_pSelectedTile != nullptr && m_pSelectedAgent != nullptr)
        {
            if (GetKey(Keyboard::LSHIFT) == false)
                m_pSelectedAgent->ResetPaths();
            gce::Vector2i32 target = {m_pSelectedTile->data->position.x, m_pSelectedTile->data->position.y};
            m_pSelectedAgent->AddPath(target);
        }
    }

    // Toggle Patrol Mode
    if (GetKeyDown(Keyboard::P))
    {
        if (m_pSelectedAgent != nullptr)
            m_pSelectedAgent->ToggleLoop();
    }

    // Display Path preview
    if (GetKey(Keyboard::TAB))
    {
        if (m_pSelectedAgent != nullptr && m_pSelectedTile != nullptr)
        {
            gce::Vector2i32 target = {m_pSelectedTile->data->position.x, m_pSelectedTile->data->position.y};
            m_pSelectedAgent->PreviewPath(target);
        }
    }

    // Delete Agent
    if (GetKeyDown(Keyboard::BACKSPACE))
        DeleteAgent();

    // Agent Speed Factor
    if (GetKeyDown(Keyboard::NUMPAD_ADD))
    {
        if (m_pSelectedAgent != nullptr)
        {
            float32 sF = m_pSelectedAgent->GetSpeedFactor();
            m_pSelectedAgent->SetSpeedFactor(gce::Max(sF, sF + 1));
        }
    }
    if (GetKeyDown(Keyboard::NUMPAD_SUBTRACT))
    {
        if (m_pSelectedAgent != nullptr)
        {
            float32 sF = m_pSelectedAgent->GetSpeedFactor();
            m_pSelectedAgent->SetSpeedFactor(gce::Max(0, sF - 1));
        }
    }

    // Tile Vertical Movement
    if (GetKeyDown(Keyboard::RSHIFT))
    {
        if (m_pSelectedTile != nullptr)
        {
            m_pSelectedTile->data->position.height += 0.1f;
            CalculateNodes();
        }
    }
    if (GetKeyDown(Keyboard::RCONTROL))
    {
        if (m_pSelectedTile != nullptr)
        {
            m_pSelectedTile->data->position.height -= 0.1f;
            CalculateNodes();
        }
    }
}

void Grid3D::CameraControl()
{
    constexpr float RadToDeg = 180.0f / gce::PI;

	Camera* camera = GameManager::Get()->GetCamera();

	float dt = GameManager::Get()->GetDeltaTime();

    static float theta = - gce::PI / 2.0f;
    float radius = 25.0f;

    if (GetKey(Keyboard::NUMPAD4) || GetKey(Keyboard::J))
    {
        theta -= 0.8f * dt;
    }
    if (GetKeyDown(Keyboard::NUMPAD5) || GetKeyDown(Keyboard::K))
        camera->Translate({ 0.0f, -1.0f, 0.0f });
    if (GetKey(Keyboard::NUMPAD6) || GetKey(Keyboard::L))
    {
        theta += 0.8f * dt;
    }
    if (GetKeyDown(Keyboard::NUMPAD8) || GetKeyDown(Keyboard::I))
        camera->Translate({ 0.0f, 1.0f, 0.0f });

	if (GetKeyDown(Keyboard::NUMPAD1) || GetKeyDown(Keyboard::W))
	{
		m_ZoomLevel -= 1;
	}   
    if (GetKeyDown(Keyboard::NUMPAD3) || GetKeyDown(Keyboard::C))
    {
		m_ZoomLevel += 1;
    }

    if (GetKeyDown(Keyboard::SPACE)) //Reset Camera Pos
        theta = -gce::PI / 2.0f;

    float x = cosf(theta) * radius;
    float z = sinf(theta) * radius;

    camera->SetPosition({ x, camera->GetPosition().y, z + 2.5f });
	camera->SetFOV(gce::PI / 4.0f - m_ZoomLevel * 0.05f); //Zoom    

    float yaw = -theta - gce::PI / 2.0f;
	float yawDeg = yaw * RadToDeg;
    camera->SetRotation({ 45.0f, yawDeg, 0.0f });
}

void Grid3D::Init(const int configIndex)
{
    GridConfig3D::LoadConfigs();

    std::vector<std::vector<char>> GRID = GridConfig3D::GetConfig(configIndex);
    int HEIGHT = GRID.size();
    int WIDTH = GRID[0].size();

    if (GRID.empty()) return;
    m_vData.clear();
    
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
            {
                tempTile.isWalkable = true;
                tempTile.position.height = float32(GRID[i][j] - '0') / 5.0f;
            }
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

    float offsetX = - m_gridSize.x * MyBlockSize * 0.5f + MyBlockSize * 0.5f;
    float offsetY = m_gridSize.y * MyBlockSize * 0.5f + MyBlockSize * 0.5f;
    Grid3D::m_anchorPoint = gce::Vector3f32(offsetX, 0.0f, offsetY);

    m_pSelectedTile = GetNode(Position(0, 0));
    cursorPos = gce::Vector2i32(0, 0);
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

        gce::Vector3f32 color = {0.0f, 1.0f, 0.0f};

        int totalIndex = n->data->position.x + n->data->position.y;
        if (totalIndex % 2 == 0)
            color = {0.0f, 0.7f, 0.0f};
        
        if (n->data->pOccupyingAgent != nullptr)
            color = {1.0f, 1.0f, 1.0f};

        if (n == m_pSelectedTile)
            color = {0.0f, 0.0f, 1.0f};
        if (n == m_pSelectedTile && n->data->isWalkable == false)
            color = {1.0f, 0.0f, 0.0f};

    if (n->data->isWalkable || n == m_pSelectedTile)
        Debug::DrawCube({m_anchorPoint.x + p.x * MyBlockSize, n->data->position.height,m_anchorPoint.z - p.y * MyBlockSize},
            {MyBlockSize - 0.2f, MyBlockSize - 0.2f, MyBlockSize - 0.2f}, color);
    }

    for (Agent3D* agent : m_vAgents)
        agent->DrawPaths();
}

void Grid3D::CreateAgent()
{
    if (m_pSelectedTile == nullptr)
        return;
    
    gce::Vector2i32 pos = {m_pSelectedTile->data->position.x, m_pSelectedTile->data->position.y};
    
    Node<Tile>* tempNode = GetNode(pos);
    if (tempNode == nullptr)
        return;
    if (tempNode->data->isWalkable == false || tempNode->data->pOccupyingAgent != nullptr)
        return;

    
    Agent3D* tempAgent = CreateEntity<Agent3D>(1.f, gce::Vector3f32(0.0f, 0.0f, 1.0f));
    tempAgent->SetPosition(m_anchorPoint.x + pos.x * MyBlockSize, MyBlockSize * 0.5f + 1, m_anchorPoint.z - pos.y * MyBlockSize);

    m_vAgents.push_back(tempAgent);
}

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

            float32 targetHeightDiff = gce::Abs(endNode->data->position.height - neighbour->data->position.height);
            neighbour->targetDistance = neighbour->data->Distance(endNode->data) + targetHeightDiff;

            float32 heightDiff = gce::Abs(curr->data->position.height - neighbour->data->position.height);
            int newCost = curr->cost + curr->data->Distance(neighbour->data) + heightDiff;
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

gce::Vector2i32 Grid3D::GetTilePosition(gce::Vector3f32 worldPos)
{
    gce::Vector2f32 pos = { worldPos.x - m_anchorPoint.x, -(worldPos.z - m_anchorPoint.z) };
    pos /= MyBlockSize;
    
    gce::Vector2i32 result(gce::RoundToInt(pos.x), gce::RoundToInt(pos.y));

    return result;
}

gce::Vector3f32 Grid3D::GetWorldPosition(gce::Vector2i32 gridPos)
{
    return { m_anchorPoint.x + gridPos.x * MyBlockSize,
        0.0f,
        m_anchorPoint.z - gridPos.y * MyBlockSize };
}

gce::Vector3f32 Grid3D::GetWorldPosition(Position const& gridPos)
{
    return { m_anchorPoint.x + (float32)gridPos.x * MyBlockSize,
        0.0f,
        m_anchorPoint.z - (float32)gridPos.y * MyBlockSize };
}

void Grid3D::CalculateNodes()
{
    m_vNodes.clear();

    gce::Vector2i32 maxIndex = {m_vData.back().back().position.x, m_vData.back().back().position.y};
    gce::Vector2i32 minIndex = {m_vData.front().front().position.x, m_vData.front().front().position.y};
    
    int WIDTH = abs(maxIndex.x - minIndex.x) + 1;
    int HEIGHT = abs(maxIndex.y - minIndex.y) + 1;

    m_gridSize = gce::Vector2i32(WIDTH, HEIGHT) ;

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
        std::vector<Position> neighbours = Position::GetNeighbours(n->data->position, maxIndex, minIndex);

        for (int i = 0; i < neighbours.size(); i++)
        {
            Position p = neighbours[i];
            Tile* neig = &m_vData[p.y - minIndex.y][p.x - minIndex.x];

            if (neig->isWalkable == false)
                continue;
            if (n->data->Distance(neig) == 2)
            {
                Position centerPos = n->data->position;
                Tile* n1 = &m_vData[p.y - minIndex.y][centerPos.x - minIndex.x];
                Tile* n2 = &m_vData[centerPos.y - minIndex.y][p.x - minIndex.x];

                float32 height = n->data->position.height;
                float32 height1 = n1->position.height;
                float32 height2 = n2->position.height;
                float32 heightNeig = neig->position.height;
                
                if (n1->isWalkable == false || n2->isWalkable == false)
                    continue;
                if (height < heightNeig)
                {
                    if (height1 < height || height2 < height)
                        continue;
                    if (height1 > heightNeig || height2 > heightNeig)
                        continue;
                }
                else if (height > heightNeig)
                {
                    if (height1 > height || height2 > height)
                        continue;
                    if (height1 < heightNeig || height2 < heightNeig)
                        continue;
                }
                else
                {
                    float32 heightDiff1 = gce::Abs(height1 - height);
                    float32 heightDiff2 = gce::Abs(height2 - height);
                    if (heightDiff1 > StepMax)
                        continue;
                    if (heightDiff2 > StepMax)
                        continue;
                }
            }

            float32 heightDif = gce::Abs(n->data->position.height - neig->position.height);
            if (heightDif > StepMax)
                continue;

            Node<Tile>* nNeig = GetNode(neig->position);
            
            n->vNeighbours.push_back(nNeig);
        }
    }
}

Node<Tile>* Grid3D::TrySelectedTile(int x, int y)
{
    gce::Vector2i32 pos = {x, y};

    gce::Vector2i32 min = { m_vData[0][0].position.x - 1, m_vData[0][0].position.y - 1 };
    gce::Vector2i32 max = { m_vData.back().back().position.x + 1, m_vData.back().back().position.y + 1 };

    if (pos.x < min.x || pos.y < min.y) return nullptr;
    if (pos.x > max.x || pos.y > max.y) return nullptr;

    Node<Tile>* n = GetNode(pos);

    if (n != nullptr) return n;

    AddTile({ pos.x, pos.y });

    n = GetNode(pos);

    if (n == nullptr)
        int i = 0;
    return n;
}

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

    gce::Vector3f32 pos = {
        pAgent->GetPosition().x,
        pAgent->GetPosition().y,
        pAgent->GetPosition().z };

    gce::Vector2i32 currTilePos = pAgent->GetTilePosition();
    Node<Tile>* pNode = GetNode(currTilePos);
    if (pNode == nullptr) return result;

    for (int i = 0; i < directions.size(); i++)
    {
        Node<Tile>* other = GetNode(currTilePos + gce::Vector2i32(directions[i].x, directions[i].z));
        if (other == nullptr) continue;

        gce::Vector3f32 otherPos = GetWorldPosition(other->data->position);
        
        float dx = abs(otherPos.x - pos.x);
        float dy = abs(otherPos.z - pos.z);

        if (dx > MyBlockSize * 0.5f + pAgent->GetRadius())
            continue;
        if (dy > MyBlockSize * 0.5f + pAgent->GetRadius())
            continue;

        result.push_back(other);
    }
    
    return result;
}

void Grid3D::AddTile(gce::Vector2i32 pos)
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

    CalculateNodes();
}

void Grid3D::DeleteAgent()
{
    if (m_pSelectedAgent == nullptr) return;

    m_pSelectedAgent->Destroy();
    m_vAgents.erase(m_vAgents.begin() + m_selectedAgentIndex);
    if (m_selectedAgentIndex == m_vAgents.size())
        m_selectedAgentIndex -= 1;
    m_selectedAgentIndex = gce::Max(0.0f, m_selectedAgentIndex);

    if (m_vAgents.empty() == false)
    {
        m_pSelectedAgent = m_vAgents[m_selectedAgentIndex];
        m_pSelectedAgent->SetColor({1.0f, 1.0f, 0.0f});
    }
    else
    {
		m_pSelectedAgent = nullptr;
    }
    
}

void Grid3D::SaveMap()
{
    std::string filePath = "../../res/Map";
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
                temp.append(std::to_string((int)(m_vData[i][j].position.height * 5)));
            else
                temp.append("#");
        }
        temp.append("\n");
        file << temp;
    }

    file.close();
}

#endif
