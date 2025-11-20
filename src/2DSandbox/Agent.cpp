#ifndef AGENT_CPP_INCLUDED
#define AGENT_CPP_INCLUDED

#include "Agent.h"
#include "Grid.h"
#include "Utils.hpp"
#include "LightEngine/Debug.h"

#define SPEED 150.0f

void Agent::OnInitialize()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;
	
	sf::Vector2f tempPos = { mShape.getPosition().x + mShape.getRadius(), mShape.getPosition().y + mShape.getRadius() };
	sf::Vector2i worldPos = { (int)(tempPos.x), (int)(tempPos.y) };
	m_tilePosition = grid->GetTilePosition(worldPos);
	m_nextPos = m_tilePosition;
}

void Agent::OnUpdate()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;

	sf::Vector2f tempPos = { mShape.getPosition().x + mShape.getRadius(), mShape.getPosition().y + mShape.getRadius() };
	sf::Vector2i worldPos = { (int)(tempPos.x), (int)(tempPos.y) };
	sf::Vector2i oldPos = m_tilePosition;

	Node<Tile>* nextNode = GetNextNode();
	if (nextNode != nullptr)
	{
		sf::Vector2i nextPos = {nextNode->data->position.x, nextNode->data->position.y};
		if (nextPos != m_tilePosition)
			m_nextPos = nextPos;
	}
	
	m_tilePosition = grid->GetTilePosition(worldPos);
	if (m_tilePosition != oldPos && grid->GetNode(oldPos) != nullptr)
		m_tilePosition = m_nextPos;
	
	if (m_vPaths.empty()) return;

	Path& currentPath = m_vPaths.front();
	
	if (mTarget.isReached == true)
	{
		if (currentPath.detourStart == -1)
			currentPath.index++;
		else
			currentPath.detourIndex++;
	}
	
	if (mTarget.isSet == false)
		SetTarget();
	
	CheckPathAvailable();
	CheckPathOccupied(worldPos);
}

void Agent::SetTarget()
{
	mTarget.isSet = false;
	mDirection = { 0.0f, 0.0f };
	
	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();
	
	if (currentPath.index < currentPath.vPositions.size())
	{
		if (currentPath.detourStart != -1 && currentPath.index - 1 == currentPath.detourStart)
		{
			if (currentPath.detourIndex < currentPath.vDetour.size())
			{
				sf::Vector2i p = {currentPath.vDetour[currentPath.detourIndex].x, currentPath.vDetour[currentPath.detourIndex].y};
				sf::Vector2i wP = Grid::GetWorldPosition(p);
				GoToPosition(wP.x, wP.y, SPEED);
			}
			else
			{
				currentPath.vDetour.clear();
				currentPath.detourStart = -1;
				currentPath.index = currentPath.detourEnd;
				currentPath.detourEnd = -1;
				currentPath.detourIndex = 0;
			}
		}
		else
		{
			sf::Vector2i p = {currentPath.vPositions[currentPath.index].x, currentPath.vPositions[currentPath.index].y};
			sf::Vector2i wP = Grid::GetWorldPosition(p);
			GoToPosition(wP.x, wP.y, SPEED);
			std::cout << currentPath.index << std::endl;
		}
	}
	else if (currentPath.isLoop == false)
		m_vPaths.erase(m_vPaths.begin());
	else
	{
		std::reverse(currentPath.vPositions.begin(), currentPath.vPositions.end());
		currentPath.index = 0;
	}	
}

void Agent::CheckPathAvailable()
{
	if (m_vPaths.empty()) return;
	Path& currentPath = m_vPaths.front();
	if (currentPath.index == 0) return;
	
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;
	Node<Tile>* node = grid->GetNode(m_tilePosition);
	if (node == nullptr) return;

	sf::Vector2i currPos = m_tilePosition;
	std::vector<Node<Tile>*> section;
	Node<Tile>* temp = node;
	
	std::vector<sf::Vector2i> positions = currentPath.vPositions;
	int startIndex = currentPath.index - 1;
	int endIndex = currentPath.index;
	
	if (currentPath.detourStart != -1)
	{
		positions = currentPath.vDetour;
		startIndex = currentPath.detourIndex - 1;
		endIndex = currentPath.detourIndex;
		if (startIndex < 0) return;
	}
	
	sf::Vector2i intDirection = positions[endIndex] - positions[startIndex];
	Clamp(intDirection.x, -1, 1);
	Clamp(intDirection.y, -1, 1);
	
	while (currPos != positions[endIndex])
	{
		section.push_back(temp);
		currPos = currPos + intDirection;
		temp = grid->GetNode(currPos);
	}
	section.push_back(temp);
	
	if (temp->data->isWalkable == false)
	{
		if (temp == grid->GetNode(currentPath.vPositions.back()))
			m_vPaths.clear();
		else if (currentPath.isLoop == false)
		{
			sf::Vector2i vPos = m_tilePosition;
			Path p = GetPath(vPos, m_vPaths.front().vPositions.back());
			m_vPaths.erase(m_vPaths.begin());
			m_vPaths.insert(m_vPaths.begin(), p);
		}
		else
		{
			if (currentPath.detourStart == -1)
				SetDetour(currentPath.index - 1, currentPath.index + 1);
			else
				SetDetour(currentPath.detourStart, currentPath.detourStart + 2);
		}

		SetTarget();
		return;
	}
	
	for (int i = 0; i < section.size(); i++)
	{
		if (section[i]->data->isWalkable == false)
		{
			int index = 0;
			if (i != 1)
				index = 1;
				
			if (currentPath.isLoop == true)
			{
				if (currentPath.detourStart == -1)
					SetDetour(currentPath.index - 1, currentPath.index);
				else
					SetDetour(currentPath.detourStart, currentPath.detourEnd);
				SetTarget();	
				break;
			}

			sf::Vector2i vPos = {section[index]->data->position.x, section[index]->data->position.y};
			Path p = GetPath(vPos, m_vPaths.front().vPositions.back());
			m_vPaths.erase(m_vPaths.begin());
			m_vPaths.insert(m_vPaths.begin(), p);
			SetTarget();	
			break;
		}
	}
}

Node<Tile>* Agent::GetNextNode()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return nullptr;
	Node<Tile>* node = grid->GetNode(m_tilePosition);

	sf::Vector2i worldPos = { (int)GetPosition().x, (int)GetPosition().y };
	sf::Vector2i nextTilePos = worldPos + sf::Vector2i(mDirection.x * 25, mDirection.y * 25);
	sf::Vector2i tileP = Grid::GetTilePosition(nextTilePos);

	Node<Tile>* nextNode = grid->GetNode(tileP);
	if (node == nextNode) return nullptr;

	return nextNode;
}

void Agent::CheckPathOccupied(sf::Vector2i worldPos)
{
	Node<Tile>* nextNode = GetNextNode();
	if (nextNode == nullptr) return;

	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();
	
	if (mSpeed == 0.0f)
	{
		if (nextNode->data->pOccupyingAgent == nullptr || nextNode->data->pOccupyingAgent == this)
		{
			mSpeed = SPEED;
			m_StuckTimer = 0.f;
		}
		else
			m_StuckTimer += GetDeltaTime();
	}
	else
	{
		if (nextNode->data->pOccupyingAgent != nullptr && nextNode->data->pOccupyingAgent != this)
		{
			mSpeed = 0.0f;
		}
	}

	if (m_StuckTimer > 1.50f)
	{
		if (currentPath.isLoop == false)
		{
			sf::Vector2i vPos = m_tilePosition;
			Path p = GetPath(vPos, m_vPaths.front().vPositions.back());
			m_vPaths.erase(m_vPaths.begin());
			m_vPaths.insert(m_vPaths.begin(), p);
		}
		else
		{
			if (currentPath.detourStart == -1)
				SetDetour(currentPath.index - 1, currentPath.index + 1);
			else
				SetDetour(currentPath.detourStart, currentPath.detourStart + 1);
		}

		SetTarget();
		mSpeed = SPEED;
		m_StuckTimer = 0.f;
	}
}

void Agent::ResetPaths()
{
	m_vPaths.clear();
	SetTarget();
}

void Agent::AddPath(sf::Vector2i vector2)
{
	if (m_vPaths.empty() == false)
	{
		Path& currentPath = m_vPaths.front();
		if (currentPath.isLoop == true)
			return;
	}
	
	sf::Vector2i start = m_tilePosition;
	if (m_vPaths.empty() == false)
		start = m_vPaths.back().vPositions.back();

	Path p = GetPath(start, vector2);
	if (p.vPositions.empty() == false)
		m_vPaths.push_back(p);
}

void Agent::SetDetour(int startIndex, int endIndex)
{
	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();
	
	if (startIndex + 1 >= currentPath.vPositions.size()) return;
	
	sf::Vector2i start = m_tilePosition;

	endIndex = startIndex + 1;
	sf::Vector2i end = currentPath.vPositions[endIndex];
	Grid* grid = GetScene<Grid>();
	Node<Tile>* endNode = grid->GetNode(end);
	while (endNode != nullptr && endNode->data->isWalkable == false && end != currentPath.vPositions.back())
	{
		endIndex++;
		end = currentPath.vPositions[endIndex];
		endNode = grid->GetNode(end);
	}

	if (endNode->data->isWalkable == false)
	{
		ResetPaths();
		return;
	}
	
	Path p = GetPath(start, end);
	if (p.vPositions.empty() == true)
	{
		ResetPaths();
		return;
	}

	if (currentPath.detourStart != -1)
		currentPath.vDetour.erase(currentPath.vDetour.begin() + currentPath.detourIndex, currentPath.vDetour.end());
	
	for (int i = 0; i < p.vPositions.size(); i++)
	{
		currentPath.vDetour.push_back(p.vPositions[i]);
	}

	currentPath.detourStart = startIndex;
	currentPath.detourEnd = endIndex;
}

Path Agent::GetPath(sf::Vector2i start, sf::Vector2i end)
{
	Grid* grid = GetScene<Grid>();
	Node<Tile>* nEnd = grid->GetNode(end);
	Node<Tile>* nSelf = grid->GetNode(start);

	if (grid == nullptr) return Path();
	if (nSelf == nullptr) return Path();
	if (nEnd == nullptr) return Path();
	
	Node<Tile>* nResult = grid->AStar(nSelf, nEnd, this);
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
	for (int i = 0; i < m_vPaths.size(); i++)
	{
		Path& currentPath = m_vPaths[i];
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

	if (currentPath.detourStart == -1) return;

	for (int i = 0; i < currentPath.vDetour.size() - 1; i++)
	{
		sf::Vector2i p1 = {currentPath.vDetour[i].x, currentPath.vDetour[i].y};
		sf::Vector2i p2 = {currentPath.vDetour[i + 1].x, currentPath.vDetour[i + 1].y};

		sf::Vector2i wP1 = Grid::GetWorldPosition(p1);
		sf::Vector2i wP2 = Grid::GetWorldPosition(p2);
			
		Debug::DrawLine(wP1.x, wP1.y, wP2.x, wP2.y, sf::Color::Red);
		Debug::DrawCircle(wP1.x, wP1.y, 5.f, sf::Color::Magenta);
		Debug::DrawCircle(wP2.x, wP2.y, 5.f, sf::Color::Magenta);
	}
	p = {currentPath.vDetour.back().x, currentPath.vDetour.back().y};
	wP = Grid::GetWorldPosition(p);
	Debug::DrawCircle(wP.x, wP.y, 5.f, sf::Color::Magenta);
}

void Agent::PreviewPath(sf::Vector2i vector)
{
	Path p = GetPath(m_tilePosition, vector);
	DrawSinglePath(p, sf::Color::White);
}

void Agent::ToggleLoop()
{
	if (m_vPaths.empty() == true) return;

	if (m_vPaths.front().isLoop == true)
		m_vPaths.front().isLoop = false;
	else
	{
		Path newPath = m_vPaths.front();
		newPath.isLoop = true;
		if (m_vPaths.size() > 1)
		{
			for (int i = 1; i < m_vPaths.size(); i++)
			{
				for (int j = 1; j < m_vPaths[i].vPositions.size(); j++)
					newPath.vPositions.push_back(m_vPaths[i].vPositions[j]);
			}
		}
		m_vPaths.clear();
		m_vPaths.push_back(newPath);
	}
}

#endif
