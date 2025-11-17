#ifndef AGENT_H_INCLUDED

#define AGENT_H_INCLUDED

#include <queue>

#include "Node.hpp"
#include "NodeData.hpp"
#include "Entity.h"

struct Path
{
	bool isLoop = false;
	int index = 0;
	std::vector<sf::Vector2i> vPositions;

	int detourStart = -1;
	int detourEnd = -1;
	int detourIndex = 0;
	std::vector<sf::Vector2i> vDetour;
};


class Agent : public Entity
{
	sf::Vector2i m_tilePosition;
	sf::Vector2i m_nextPos;
	std::vector<Path> m_vPaths;
	float m_StuckTimer;
		
public:
	Agent() = default;
	~Agent() = default;

	Node<Tile>* GetNextNode();
	
	void CheckPathOccupied(sf::Vector2i worldPos);
	void SetTarget();
	void CheckPathAvailable();

	void OnUpdate();
	void OnCollision(Entity* collidedWith) {}

	sf::Vector2i GetTilePosition() { return m_tilePosition; }
	sf::Vector2i GetNextTilePosition() { return m_nextPos; }
	
	void ResetPaths();
	void AddPath(sf::Vector2i vector2);
	void SetDetour(int startIndex, int endIndex);
	Path GetPath(sf::Vector2i start, sf::Vector2i end);
	void DrawPaths();
	void DrawSinglePath(Path& path, sf::Color color = sf::Color::Blue);
	void PreviewPath(sf::Vector2i vector);
	
	void OnInitialize();
	void OnDestroy() {}
	void ToggleLoop();
};

#endif 








