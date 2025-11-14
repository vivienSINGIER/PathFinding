#ifndef AGENT_CPP_INCLUDED
#define AGENT_CPP_INCLUDED

#include "Agent.h"
#include "Grid.h"
#include "Utils.hpp"
#include "../Debug.h"

#define SPEED 80.0f


void Agent::OnUpdate()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;

	sf::Vector2f tempPos = { mShape.getPosition().x + mShape.getRadius(), mShape.getPosition().y + mShape.getRadius() };
	sf::Vector2i worldPos = { (int)(tempPos.x), (int)(tempPos.y) };
	m_tilePosition = grid->GetTilePosition(worldPos);
	
	if (vPaths.empty()) return;

	if (mTarget.isSet == false)
		SetTarget();

	CheckPathOccupied(worldPos);
	CheckPathAvailable(worldPos);
}

void Agent::SetTarget()
{
	CheckPathAvailable(Grid::GetWorldPosition(m_tilePosition));
	Path& currentPath = vPaths.front();

	if (currentPath.vPositions.empty() == true) return;

	
	if (currentPath.index < currentPath.vPositions.size())
	{
		sf::Vector2i p = {currentPath.vPositions[currentPath.index].x, currentPath.vPositions[currentPath.index].y};
		sf::Vector2i wP = Grid::GetWorldPosition(p);
		GoToPosition(wP.x, wP.y, SPEED);
		currentPath.index++;
	}
	else if (currentPath.isLoop == false)
		vPaths.erase(vPaths.begin());
	else if (currentPath.isLoop == true)
	{
		std::reverse(currentPath.vPositions.begin(), currentPath.vPositions.end());
		currentPath.index = 0;
	}
}

void Agent::CheckPathAvailable(sf::Vector2i worldPos)
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;

	Node<Tile>* node = grid->GetNode(m_tilePosition);
	if (node == nullptr) return;
	
	sf::Vector2i currPos = m_tilePosition;
	std::vector<Node<Tile>*> section;
	Node<Tile>* temp = node;

	Node<Tile>* next = GetNextNode(worldPos);
	if (next == nullptr) return;

	sf::Vector2i nextPos = {next->data->position.x, next->data->position.y};
	
	sf::Vector2i intDirection = nextPos - m_tilePosition;
	
	while (currPos != Grid::GetTilePosition(mTarget.position))
	{
		section.push_back(temp);
		currPos = currPos + intDirection;
		temp = grid->GetNode(currPos);
	}

	for (int i = 0; i < section.size(); i++)
	{
		if (section[i]->data->isWalkable == false)
		{
			int index = 0;
			
			if (i != 1)
				index = 1;

			sf::Vector2i vPos = {section[index]->data->position.x, section[index]->data->position.y};
			Path p = GetPath(vPos, vPaths.front().vPositions.back());
			vPaths.erase(vPaths.begin());
			vPaths.insert(vPaths.begin(), p);
			mTarget.isSet = false;
			break;
		}
	}
}

Node<Tile>* Agent::GetNextNode(sf::Vector2i worldPos)
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return nullptr;
	Node<Tile>* node = grid->GetNode(m_tilePosition);
	
	sf::Vector2i nextTilePos = worldPos + sf::Vector2i(mDirection.x * 25, mDirection.y * 25);
	sf::Vector2i tileP = Grid::GetTilePosition(nextTilePos);
	
	Node<Tile>* nextNode = grid->GetNode(tileP);
	if (node == nextNode) return nullptr;

	return nextNode;
}

void Agent::CheckPathOccupied(sf::Vector2i worldPos)
{
	Node<Tile>* nextNode = GetNextNode(worldPos);
	if (nextNode == nullptr) return;
	
	if (mSpeed == 0.0f)
	{
		if (nextNode->data->isOccupied == false)
		{
			mSpeed = SPEED;
			m_StuckTimer = 0.f;
		}
		else
			m_StuckTimer += GetDeltaTime();
	}
	else
	{
		if (nextNode->data->isOccupied == true)
		{
			mSpeed = 0.0f;
		}
	}
}

void Agent::ResetPaths()
{
	mTarget.isSet = false;
	vPaths.clear();
}

void Agent::AddPath(sf::Vector2i vector2)
{
	sf::Vector2i start = m_tilePosition;
	if (vPaths.empty() == false)
		start = vPaths.back().vPositions.back();

	Path p = GetPath(start, vector2);
	if (p.vPositions.empty() == false)
		vPaths.push_back(p);
}

void Agent::AddTempPath(sf::Vector2i vector2)
{

}

Path Agent::GetPath(sf::Vector2i start, sf::Vector2i end)
{
	Grid* grid = GetScene<Grid>();
	Node<Tile>* nEnd = grid->GetNode(end);
	Node<Tile>* nSelf = grid->GetNode(start);

	if (grid == nullptr) return Path();
	if (nSelf == nullptr) return Path();
	if (nEnd == nullptr) return Path();

	if (vPaths.empty() == false)
	{
		Path& currentPath = vPaths.front();
		if (currentPath.isLoop == true)
			return Path();
	}
	
	Node<Tile>* nResult = grid->AStar(nSelf, nEnd);
	if (nResult == nullptr)
		return Path();
	
	Path path;
	
	while (nResult != nullptr)
	{
		sf::Vector2i p = {nResult->data->position.x, nResult->data->position.y};
		path.vPositions.insert(path.vPositions.begin(), p);
		nResult = nResult->pCameFrom;
	}

	if (path.vPositions.size() > 1)
	{
		sf::Vector2i dir = {0, 0};
		int i = 1;
		while (i < path.vPositions.size())
		{
			sf::Vector2i newDir = path.vPositions[i] - path.vPositions[i - 1];
			SelfNormalize(newDir);
			while (newDir == dir && i < path.vPositions.size())
			{
				path.vPositions.erase(path.vPositions.begin() + i - 1);
				if (i < path.vPositions.size())
				{
					newDir = path.vPositions[i] - path.vPositions[i - 1];
					SelfNormalize(newDir);	
				}
			}
			dir = newDir;
			i++;
		}
	}

	return path;
}

void Agent::DrawPaths()
{
	for (int i = 0; i < vPaths.size(); i++)
	{
		Path& currentPath = vPaths[i];
		if (currentPath.isLoop == true)
			DrawSinglePath(currentPath, sf::Color(255, 145, 0));
		else
			DrawSinglePath(currentPath);
	}
}

void Agent::DrawSinglePath(Path& path, sf::Color color)
{
	Path& currentPath = path;

	if (path.vPositions.empty() == true) return;
	
	for (int i = 0; i < currentPath.vPositions.size() - 1; i++)
	{
		sf::Vector2i p1 = {currentPath.vPositions[i].x, currentPath.vPositions[i].y};
		sf::Vector2i p2 = {currentPath.vPositions[i + 1].x, currentPath.vPositions[i + 1].y};

		sf::Vector2i wP1 = Grid::GetWorldPosition(p1);
		sf::Vector2i wP2 = Grid::GetWorldPosition(p2);
			
		Debug::DrawLine(wP1.x, wP1.y, wP2.x, wP2.y, color);
		Debug::DrawCircle(wP1.x, wP1.y, 5.f, sf::Color::Magenta);
		Debug::DrawCircle(wP2.x, wP2.y, 5.f, sf::Color::Magenta);
	}
	sf::Vector2i p = {currentPath.vPositions.back().x, currentPath.vPositions.back().y};
	sf::Vector2i wP = Grid::GetWorldPosition(p);
		
	Debug::DrawCircle(wP.x, wP.y, 5.f, sf::Color::Magenta);
}

void Agent::PreviewPath(sf::Vector2i vector)
{
	Path p = GetPath(m_tilePosition, vector);
	DrawSinglePath(p, sf::Color::White);
}

void Agent::OnInitialize()
{

}

void Agent::ToggleLoop()
{
	if (vPaths.empty() == true) return;

	if (vPaths.front().isLoop == true)
		vPaths.front().isLoop = false;
	else
	{
		Path newPath = vPaths.front();
		newPath.isLoop = true;
		if (vPaths.size() > 1)
		{
			for (int i = 1; i < vPaths.size(); i++)
			{
				for (int j = 1; j < vPaths[i].vPositions.size(); j++)
					newPath.vPositions.push_back(vPaths[i].vPositions[j]);
			}
		}
		vPaths.clear();
		vPaths.push_back(newPath);
	}
}

#endif
