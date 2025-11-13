#ifndef AGENT_H_INCLUDED

#define AGENT_H_INCLUDED

#include <queue>

#include "Node.hpp"
#include "../Entity.h"

struct Path
{
	bool isLoop = false;
	int index = 0;
	std::vector<sf::Vector2i> vPositions;
};


class Agent : public Entity
{
	sf::Vector2i m_tilePosition;
	std::vector<Path> vPaths;
		
public:
	Agent() = default;
	~Agent() = default;

	void OnUpdate();
	void OnCollision(Entity* collidedWith) {}

	sf::Vector2i GetTilePosition() { return m_tilePosition; }
	
	void ResetPaths();
	void AddPath(sf::Vector2i vector2);
	Path GetPath(sf::Vector2i start, sf::Vector2i end);
	void DrawPaths();
	void DrawSinglePath(Path& path, sf::Color color = sf::Color::Blue);
	void PreviewPath(sf::Vector2i vector);
	
	void OnInitialize();
	void OnDestroy() {}
	void ToggleLoop();
};

#endif 








