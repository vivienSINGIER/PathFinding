#include "pch.h"
#ifndef AGENT_CPP_INCLUDED
#define AGENT_CPP_INCLUDED

#include "Agent3D.h"
#include "Grid3D.h"
#include "Maths/Vector3.h"
#include "Maths/MathsFunctions.hpp"
#include "3DLightEngine/Debug.h"

#define SPEED 10.0f

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

	gce::Vector2i32 oldPos = m_tilePosition;

	Node<Tile>* nextNode = GetNextNode();
	if (nextNode != nullptr)
	{
		gce::Vector2i32 nextPos = { nextNode->data->position.x, nextNode->data->position.y };
		if (nextPos != m_tilePosition)
			m_nextPos = nextPos;
	}

	m_tilePosition = grid->GetTilePosition(m_Shape->GetPosition());
	if (m_tilePosition != oldPos && grid->GetNode(oldPos) != nullptr)
	{
		m_tilePosition = m_nextPos;
	}
	
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
				gce::Vector2i32 p = { currentPath.vDetour[currentPath.detourIndex].x, currentPath.vDetour[currentPath.detourIndex].y };
				gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
				GoToPosition(wP.x, GetPosition().y, wP.z, SPEED);
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
			gce::Vector2i32 p = { currentPath.vPositions[currentPath.index].x, currentPath.vPositions[currentPath.index].y };
			gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
			GoToPosition(wP.x, GetPosition().y, wP.z, SPEED);
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
	if (m_vPaths.empty()) return;
	Path& currentPath = m_vPaths.front();
	if (currentPath.index == 0) return;

	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return;
	Node<Tile>* node = grid->GetNode(m_tilePosition);
	if (node == nullptr) return;

	gce::Vector2i32 currPos = m_tilePosition;
	std::vector<Node<Tile>*> section;
	Node<Tile>* temp = node;

	std::vector<gce::Vector2i32> positions = currentPath.vPositions;
	int startIndex = currentPath.index - 1;
	int endIndex = currentPath.index;

	if (currentPath.detourStart != -1)
	{
		positions = currentPath.vDetour;
		startIndex = currentPath.detourIndex - 1;
		endIndex = currentPath.detourIndex;
		if (startIndex < 0) return;
	}

	gce::Vector2i32 intDirection =  positions[endIndex] - positions[startIndex];
	intDirection.x = gce::Clamp(intDirection.x, -1, 1);
	intDirection.y = gce::Clamp(intDirection.y, -1, 1);

	while (currPos != positions[endIndex])
	{
		section.push_back(temp);
		currPos = currPos + intDirection;
		temp = grid->GetNode(currPos);
	}
	section.push_back(temp);

	if (temp->data->isWalkable == false || temp->vNeighbours.empty())
	{
		if (temp == grid->GetNode(currentPath.vPositions.back()))
			m_vPaths.clear();
		else if (currentPath.isLoop == false)
		{
			gce::Vector2i32 vPos = m_tilePosition;
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
		bool obstructed = false;
		if (i > 0)
		{
			Position p1 = section[i - 1]->data->position;
			Position p2 = section[i]->data->position;
			float32 heightDif = abs(p2.height - p1.height);
			if (heightDif > StepMax)
				obstructed = true;
		}
		
		if (section[i]->data->isWalkable == false)
			obstructed = true;

		if (obstructed == false) continue;
		
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

		gce::Vector2i32 vPos = { (int8)section[index]->data->position.x, (int8)section[index]->data->position.y };
		Path p = GetPath(vPos, m_vPaths.front().vPositions.back());
		m_vPaths.erase(m_vPaths.begin());
		m_vPaths.insert(m_vPaths.begin(), p);
		SetTarget();
		break;
	}
}

Node<Tile>* Agent3D::GetNextNode()
{
	Grid3D* grid = GetScene<Grid3D>();
	if (grid == nullptr) return nullptr;
	Node<Tile>* node = grid->GetNode(m_tilePosition);

	gce::Vector3f32 worldPos = GetPosition();
	gce::Vector3f32 nextTilePos = worldPos + gce::Vector3f32(mDirection.x * GetRadius(), mDirection.y * GetRadius(), mDirection.z * GetRadius());
	gce::Vector2i32 tileP = Grid3D::GetTilePosition(nextTilePos);

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
			gce::Vector2i32 vPos = m_tilePosition;
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

void Agent3D::AddPath(gce::Vector2i32 vector2)
{
	if (m_vPaths.empty() == false)
	{
		Path& currentPath = m_vPaths.front();
		if (currentPath.isLoop == true)
			return;
	}

	gce::Vector2i32 start = m_tilePosition;
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

	gce::Vector2i32 start = m_tilePosition;

	endIndex = startIndex + 1;
	gce::Vector2i32 end = currentPath.vPositions[endIndex];
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

Path Agent3D::GetPath(gce::Vector2i32 start, gce::Vector2i32 end)
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
		gce::Vector2i32 p = { (int8)nResult->data->position.x, (int8)nResult->data->position.y };
		path.vPositions.insert(path.vPositions.begin(), p);
		nResult = nResult->pCameFrom;
	}

	if (path.vPositions.size() > 1)
	{
		gce::Vector2i32 dir = { 0, 0 };
		int i = 1;
		while (i < path.vPositions.size())
		{
			gce::Vector2i32 newDir = path.vPositions[i] - path.vPositions[i - 1];
			newDir.SelfNormalize();
			while (newDir == dir && i < path.vPositions.size())
			{
				path.vPositions.erase(path.vPositions.begin() + i - 1);
				if (i < path.vPositions.size())
				{
					newDir = path.vPositions[i] - path.vPositions[i - 1];
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
			DrawSinglePath(currentPath, {1.0f, 145.f / 255.0f, 0.0f});
		else
			DrawSinglePath(currentPath);
	}
}

void Agent3D::DrawSinglePath(Path& path, gce::Vector3f32 color)
{
	Path& currentPath = path;
	
	if (path.vPositions.empty() == true) return;
	Grid3D* grid = GetScene<Grid3D>();
	
	float32 halfBlock = MyBlockSize * 0.5f;
	
	for (int i = 0; i < currentPath.vPositions.size() - 1; i++)
	{
		gce::Vector2i32 p1 = { currentPath.vPositions[i].x, currentPath.vPositions[i].y };
		gce::Vector2i32 p2 = { currentPath.vPositions[i + 1].x, currentPath.vPositions[i + 1].y };

		gce::Vector3f32 wP1 = Grid3D::GetWorldPosition(p1);
		gce::Vector3f32 wP2 = Grid3D::GetWorldPosition(p2);

		float32 height1 = grid->GetNode(p1)->data->position.height;
		float32 height2 = grid->GetNode(p2)->data->position.height;
		
		Debug::DrawLine({wP1.x, height1 + halfBlock + GetRadius(), wP1.z},
			{wP2.x, height2 + halfBlock + GetRadius(), wP2.z}, color);
		Debug::DrawSphere({wP1.x, height1 + halfBlock + GetRadius(), wP1.z},
			0.5, { 1.0f, 0.0f, 1.0f });
		Debug::DrawSphere({wP2.x, height2 + halfBlock + GetRadius(), wP2.z},
			0.5, { 1.0f, 0.0f, 1.0f });
	}
	gce::Vector2i32 p = { currentPath.vPositions.back().x, currentPath.vPositions.back().y };
	gce::Vector3f32 wP = Grid3D::GetWorldPosition(p);
	float32 height = grid->GetNode(p)->data->position.height;
	Debug::DrawSphere({wP.x, height + halfBlock + GetRadius(), wP.z},
			0.5, { 1.0f, 0.0f, 1.0f });

	if (currentPath.detourStart == -1) return;

	for (int i = 0; i < currentPath.vDetour.size() - 1; i++)
	{
		gce::Vector2i32 p1 = { currentPath.vDetour[i].x, currentPath.vDetour[i].y };
		gce::Vector2i32 p2 = { currentPath.vDetour[i + 1].x, currentPath.vDetour[i + 1].y };

		gce::Vector3f32 wP1 = Grid3D::GetWorldPosition(p1);
		gce::Vector3f32 wP2 = Grid3D::GetWorldPosition(p2);

		float32 height1 = grid->GetNode(p1)->data->position.height;
		float32 height2 = grid->GetNode(p2)->data->position.height;
		
		Debug::DrawLine({wP1.x, height1 + halfBlock + GetRadius(), wP1.z},
			{wP2.x, height2 + halfBlock + GetRadius(), wP2.z}, color);
		Debug::DrawSphere({wP1.x, height1 + halfBlock + GetRadius(), wP1.z},
			0.5, { 1.0f, 0.0f, 1.0f });
		Debug::DrawSphere({wP2.x, height2 + halfBlock + GetRadius(), wP2.z},
			0.5, { 1.0f, 0.0f, 1.0f });
	}
	p = { currentPath.vDetour.back().x, currentPath.vDetour.back().y };
	wP = Grid3D::GetWorldPosition(p);
	height = grid->GetNode(p)->data->position.height;
	Debug::DrawSphere({wP.x, height + halfBlock + GetRadius(), wP.z},
			0.5, { 1.0f, 0.0f, 1.0f });
}

void Agent3D::PreviewPath(gce::Vector2i32 vector)
{
	Path p = GetPath(m_tilePosition, vector);
	DrawSinglePath(p, {1.0f, 1.0f, 1.0f});
}

void Agent3D::UpdateHeight(float32 height)
{
	m_Shape->SetPosition({GetPosition().x, height + 1.25f + GetRadius(), GetPosition().z});
	mTarget.position.y = height + 1.25f + GetRadius();
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
