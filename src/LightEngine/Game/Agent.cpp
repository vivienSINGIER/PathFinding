#ifndef AGENT_CPP_INCLUDED
#define AGENT_CPP_INCLUDED

#include "Agent.h"
#include "Grid.h"
#include "Utils.hpp"
#include "../Debug.h"

void Agent::OnUpdate()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;

	sf::Vector2f tempPos = { mShape.getPosition().x, mShape.getPosition().y };
	sf::Vector2i worldPos = { (int)(tempPos.x), (int)(tempPos.y) };
	m_tilePosition = grid->GetTilePosition(worldPos);

	Node<Tile>* tempNode = grid->GetNode(m_tilePosition);
	if (tempNode != nullptr)
		tempNode->data->isOccupied = true;

	if (vPaths.empty()) return;

	Path& currentPath = vPaths.front();
	
	if (mTarget.isSet == false)
	{
		if (currentPath.index != currentPath.vPositions.size())
		{
			sf::Vector2i p = {currentPath.vPositions.front().x, currentPath.vPositions.front().y};
			sf::Vector2i wP = grid->GetWorldPosition(p);
			GoToPosition(wP.x, wP.y, 50.0f);
			currentPath.index++;
		}
		else if (currentPath.isLoop == false)
			vPaths.pop();
		else if (currentPath.isLoop == true)
		{
			std::reverse(currentPath.vPositions.begin(), currentPath.vPositions.end());
			currentPath.index = 0;
		}
	}
}

void Agent::ResetPaths()
{
	while (vPaths.empty() == false)
		vPaths.pop();
}

void Agent::AddPath(sf::Vector2i vector2)
{
	Grid* grid = GetScene<Grid>();
	Node<Tile>* nEnd = grid->GetNode(vector2);
	Node<Tile>* nSelf = grid->GetNode(m_tilePosition);

	if (grid == nullptr) return;
	if (nSelf == nullptr) return;
	if (nEnd == nullptr) return;

	if (vPaths.empty() == false)
	{
		Path& currentPath = vPaths.front();
		if (currentPath.isLoop == true)
			return;
	}
	
	Node<Tile>* nResult = grid->AStar(nSelf, nEnd);
	if (nResult == nullptr)
		return;
	
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
		for (int i = 1; i < path.vPositions.size(); i++)
		{
			sf::Vector2i newDir = path.vPositions[i] - path.vPositions[i - 1];
			SelfNormalize(newDir);
			if (newDir == dir)
				path.vPositions.erase(path.vPositions.begin() + i);
			dir = newDir;
		}
	}

	vPaths.push(path);
}

void Agent::DrawPaths()
{
	std::queue<Path> temp = vPaths;

	while (temp.empty() == false)
	{
		Path& currentPath = temp.front();
		for (int i = 0; i < currentPath.vPositions.size() - 1; i++)
		{
			sf::Vector2i p1 = {currentPath.vPositions[i].x, currentPath.vPositions[i].y};
			sf::Vector2i p2 = {currentPath.vPositions[i + 1].x, currentPath.vPositions[i + 1].y};

			Debug::DrawLine(p1.x, p1.y, p2.x, p2.y, sf::Color::White);
			Debug::DrawCircle(p1.x, p1.y, 10.f, sf::Color::Magenta);
			Debug::DrawCircle(p2.x, p2.y, 10.f, sf::Color::Magenta);
		}
		sf::Vector2i p = {currentPath.vPositions.back().x, currentPath.vPositions.back().y};
		Debug::DrawCircle(p.x, p.y, 10.f, sf::Color::Magenta);
		temp.pop();
	}
}

void Agent::OnInitialize()
{

}

#endif