#ifndef AGENT_CPP_INCLUDED
#define AGENT_CPP_INCLUDED

#include "Agent3D.h"
#include "Grid3D.h"
#include "../3DLightEngine/Utils.h"
#include "Maths/Vector3.h"
#include "Maths/MathsFunctions.hpp"
//#include "LightEngine/Debug.h"

#define SPEED 80.0f

void Agent3D::OnInitialize()
{
	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return;

	m_tilePosition = grid->GetTilePosition(m_Shape->GetPosition());
	m_nextPos = m_tilePosition;
}

void Agent3D::OnUpdate()
{
	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return;

	gce::Vector2i8 oldPos = m_tilePosition;

	Node<Tile>* nextNode = GetNextNode();
	if (nextNode != nullptr)
	{
		gce::Vector2i8 nextPos = { (int8)nextNode->data->position.x, (int8)nextNode->data->position.y };
		if (nextPos != m_tilePosition)
			m_nextPos = nextPos;
	}

	m_tilePosition = grid->GetTilePosition(m_Shape->GetPosition());
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
	CheckPathOccupied();
}

void Agent3D::SetTarget()
{
	mTarget.isSet = false;
	mDirection = { 0.0f, 0.0f, 0.0f };

	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();

	if (currentPath.index < currentPath.vPositions.size())
	{
		if (currentPath.detourStart != -1 && currentPath.index - 1 == currentPath.detourStart)
		{
			if (currentPath.detourIndex < currentPath.vDetour.size())
			{
				gce::Vector2i8 p = { currentPath.vDetour[currentPath.detourIndex].x, currentPath.vDetour[currentPath.detourIndex].y };
				gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
				GoToPosition(wP.x, wP.y, wP.z, SPEED);
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
			gce::Vector2i8 p = { currentPath.vPositions[currentPath.index].x, currentPath.vPositions[currentPath.index].y };
			gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
			GoToPosition(wP.x, wP.y, wP.z, SPEED);
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

void Agent3D::CheckPathAvailable()
{
	gce::Vector2i16 test1 = {0,0};
	gce::Vector2i16 test2 = { 0,0 };

	gce::Vector2i16 test3 = test1 + test2;

	if (m_vPaths.empty()) return;
	Path& currentPath = m_vPaths.front();
	if (currentPath.index == 0) return;

	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return;
	Node<Tile>* node = grid->GetNode(m_tilePosition);
	if (node == nullptr) return;

	gce::Vector2i8 currPos = m_tilePosition;
	std::vector<Node<Tile>*> section;
	Node<Tile>* temp = node;

	std::vector<gce::Vector2i8> positions = currentPath.vPositions;
	int startIndex = currentPath.index - 1;
	int endIndex = currentPath.index;

	if (currentPath.detourStart != -1)
	{
		positions = currentPath.vDetour;
		startIndex = currentPath.detourIndex - 1;
		endIndex = currentPath.detourIndex;
		if (startIndex < 0) return;
	}

	gce::Vector2i8 intDirection = { 0,0 }; // positions[endIndex] - positions[startIndex];
	gce::Clamp(intDirection.x, -1, 1);
	gce::Clamp(intDirection.y, -1, 1);

	while (currPos != positions[endIndex])
	{
		section.push_back(temp);
		currPos = { 0,0 }; //currPos + intDirection;
		temp = grid->GetNode(currPos);
	}

	if (temp->data->isWalkable == false)
	{
		if (temp == grid->GetNode(currentPath.vPositions.back()))
			m_vPaths.clear();
		if (currentPath.isLoop == false)
		{
			gce::Vector2i8 vPos = m_tilePosition;
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

			if (i == section.size() - 1)
				int o = 0;

			if (currentPath.isLoop == true)
			{
				if (currentPath.detourStart == -1)
					SetDetour(currentPath.index - 1, currentPath.index);
				else
					SetDetour(currentPath.detourStart, currentPath.detourEnd);
				SetTarget();
				break;
			}

			gce::Vector2i8 vPos = { (int8)section[index]->data->position.x, (int8)section[index]->data->position.y };
			Path p = GetPath(vPos, m_vPaths.front().vPositions.back());
			m_vPaths.erase(m_vPaths.begin());
			m_vPaths.insert(m_vPaths.begin(), p);
			SetTarget();
			break;
		}
	}
}

Node<Tile>* Agent3D::GetNextNode()
{
	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return nullptr;
	Node<Tile>* node = grid->GetNode(m_tilePosition);

	gce::Vector3f32 worldPos = { GetPosition() };
	gce::Vector3f32 nextTilePos = worldPos + gce::Vector3f32(mDirection.x * 25, mDirection.y * 25, mDirection.z * 25);
	gce::Vector2i8 tileP = Grid3D::GetTilePosition(nextTilePos);

	Node<Tile>* nextNode = grid->GetNode(tileP);
	if (node == nextNode) return nullptr;

	return nextNode;
}

void Agent3D::CheckPathOccupied()
{
	Node<Tile>* nextNode = GetNextNode();
	if (nextNode == nullptr) return;

	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();

	if (mSpeed == 0.0f)
	{
		if (nextNode->data->pOccupyingAgent == nullptr)
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
			gce::Vector2i8 vPos = m_tilePosition;
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

void Agent3D::ResetPaths()
{
	m_vPaths.clear();
	SetTarget();
}

void Agent3D::AddPath(gce::Vector2i8 vector2)
{
	if (m_vPaths.empty() == false)
	{
		Path& currentPath = m_vPaths.front();
		if (currentPath.isLoop == true)
			return;
	}

	gce::Vector2i8 start = m_tilePosition;
	if (m_vPaths.empty() == false)
		start = m_vPaths.back().vPositions.back();

	Path p = GetPath(start, vector2);
	if (p.vPositions.empty() == false)
		m_vPaths.push_back(p);
}

void Agent3D::SetDetour(int startIndex, int endIndex)
{
	if (m_vPaths.empty() == true) return;
	Path& currentPath = m_vPaths.front();

	if (startIndex + 1 >= currentPath.vPositions.size()) return;

	gce::Vector2i8 start = m_tilePosition;

	endIndex = startIndex + 1;
	gce::Vector2i8 end = currentPath.vPositions[endIndex];
	Grid3D* grid = GetScene<Grid3D>();
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
	if (p.vPositions.empty() == true) return;

	if (currentPath.detourStart != -1)
		currentPath.vDetour.erase(currentPath.vDetour.begin() + currentPath.detourIndex, currentPath.vDetour.end());

	for (int i = 0; i < p.vPositions.size(); i++)
	{
		currentPath.vDetour.push_back(p.vPositions[i]);
	}

	currentPath.detourStart = startIndex;
	currentPath.detourEnd = endIndex;
}

Path Agent3D::GetPath(gce::Vector2i8 start, gce::Vector2i8 end)
{
	Grid3D* grid = GetScene<Grid3D>();
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
		gce::Vector2i8 p = { (int8)nResult->data->position.x, (int8)nResult->data->position.y };
		path.vPositions.insert(path.vPositions.begin(), p);
		nResult = nResult->pCameFrom;
	}

	if (path.vPositions.size() > 1)
	{
		gce::Vector2i8 dir = { 0, 0 };
		int i = 1;
		while (i < path.vPositions.size())
		{
			gce::Vector2i8 newDir = { 0,0 }; //path.vPositions[i] - path.vPositions[i - 1];
			newDir.SelfNormalize();
			while (newDir == dir && i < path.vPositions.size())
			{
				path.vPositions.erase(path.vPositions.begin() + i - 1);
				if (i < path.vPositions.size())
				{
					newDir = { 0,0 }; //path.vPositions[i] - path.vPositions[i - 1];
					newDir.SelfNormalize();
				}
			}
			dir = newDir;
			i++;
		}
	}

	return path;
}

void Agent3D::DrawPaths()
{
	for (int i = 0; i < m_vPaths.size(); i++)
	{
		Path& currentPath = m_vPaths[i];
		if (currentPath.isLoop == true)
			DrawSinglePath(currentPath, gce::Color(255, 145, 0));
		else
			DrawSinglePath(currentPath);
	}
}

void Agent3D::DrawSinglePath(Path& path, gce::Color color)
{
	Path& currentPath = path;

	if (path.vPositions.empty() == true) return;

	for (int i = 0; i < currentPath.vPositions.size() - 1; i++)
	{
		gce::Vector2i8 p1 = { currentPath.vPositions[i].x, currentPath.vPositions[i].y };
		gce::Vector2i8 p2 = { currentPath.vPositions[i + 1].x, currentPath.vPositions[i + 1].y };

		gce::Vector3f32 wP1 = Grid3D::GetWorldPosition(p1);
		gce::Vector3f32 wP2 = Grid3D::GetWorldPosition(p2);

		//Debug::DrawLine(wP1.x, wP1.y, wP2.x, wP2.y, color);
		//Debug::DrawCircle(wP1.x, wP1.y, 5.f, gce::Color::Magenta);
		//Debug::DrawCircle(wP2.x, wP2.y, 5.f, gce::Color::Magenta);
	}
	gce::Vector2i8 p = { currentPath.vPositions.back().x, currentPath.vPositions.back().y };
	gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
	//Debug::DrawCircle(wP.x, wP.y, 5.f, gce::Color::Magenta);

	if (currentPath.detourStart == -1) return;

	for (int i = 0; i < currentPath.vDetour.size() - 1; i++)
	{
		gce::Vector2i8 p1 = { currentPath.vDetour[i].x, currentPath.vDetour[i].y };
		gce::Vector2i8 p2 = { currentPath.vDetour[i + 1].x, currentPath.vDetour[i + 1].y };

		gce::Vector3f32 wP1 = Grid3D::GetWorldPosition(p1);
		gce::Vector3f32 wP2 = Grid3D::GetWorldPosition(p2);

		//Debug::DrawLine(wP1.x, wP1.y, wP2.x, wP2.y, gce::Color::Red);
		//Debug::DrawCircle(wP1.x, wP1.y, 5.f, gce::Color::Magenta);
		//Debug::DrawCircle(wP2.x, wP2.y, 5.f, gce::Color::Magenta);
	}
	p = { currentPath.vDetour.back().x, currentPath.vDetour.back().y };
	wP = Grid3D::GetWorldPosition(p);
	//Debug::DrawCircle(wP.x, wP.y, 5.f, gce::Color::Magenta);
}

void Agent3D::PreviewPath(gce::Vector2i8 vector)
{
	Path p = GetPath(m_tilePosition, vector);
	DrawSinglePath(p, gce::Color::White);
}

void Agent3D::ToggleLoop()
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
