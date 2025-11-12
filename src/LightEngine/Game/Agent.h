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
	std::queue<Path> vPaths;
		
public:
	Agent() = default;
	~Agent() = default;

	void OnUpdate();
	void OnCollision(Entity* collidedWith) {}
	
	void ResetPaths();
	void AddPath(sf::Vector2i vector2);
	void DrawPaths();
	
	void OnInitialize();
	void OnDestroy() {};
};

#endif 








