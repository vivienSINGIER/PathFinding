#ifndef AGENT3D_H_INCLUDED

#define AGENT3D_H_INCLUDED

#include <queue>

#include "Node3D.hpp"
#include "NodeData3D.hpp"
#include "3DLightEngine/Entity.h"

struct Path
{
	bool isLoop = false;
	int index = 0;
	std::vector<gce::Vector2i32> vPositions;

	int detourStart = -1;
	int detourEnd = -1;
	int detourIndex = 0;
	std::vector<gce::Vector2i32> vDetour;
};

class Agent3D : public Entity
{
	gce::Vector2i32 m_tilePosition;
	gce::Vector2i32 m_nextPos;
	std::vector<Path> m_vPaths;
	float m_StuckTimer;

public:
	Agent3D() = default;
	~Agent3D() = default;

	Node<Tile>* GetNextNode();

	void CheckPathOccupied();
	void SetTarget();
	void CheckPathAvailable();

	void OnUpdate();
	void OnCollision(Entity* collidedWith) {}

	gce::Vector2i32 GetTilePosition() { return m_tilePosition; }
	gce::Vector2i32 GetNextTilePosition() { return m_nextPos; }

	void ResetPaths();
	void AddPath(gce::Vector2i32 vector2);
	void SetDetour(int startIndex, int endIndex);
	Path GetPath(gce::Vector2i32 start, gce::Vector2i32 end);
	void DrawPaths();
	void DrawSinglePath(Path& path, gce::Vector3f32 color = {0.0f, 0.0f, 1.0f});
	void PreviewPath(gce::Vector2i32 vector);
	void UpdateHeight(float32 height);

	void OnInitialize();
	void OnDestroy() {}
	void ToggleLoop();
};

#endif 








