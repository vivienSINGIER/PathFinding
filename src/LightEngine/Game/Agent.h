#ifndef AGENT_H_INCLUDED

#define AGENT_H_INCLUDED

#include "../Entity.h"


class Agent : public Entity
{
	sf::Vector2i m_tilePosition;

public:
	Agent() = default;
	~Agent() = default;

	void OnUpdate();
	void OnCollision(Entity* collidedWith) {};
	void OnInitialize();
	void OnDestroy() {};
};

#endif 








