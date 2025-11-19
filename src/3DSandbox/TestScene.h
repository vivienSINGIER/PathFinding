#pragma once
#include <3DLightEngine/Scene.h>

class Entity;

class TestScene : public Scene
{
	Entity* m_pEntity1;
	Entity* m_pEntity2;

	protected:
	void OnInitialize() override;
	//void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;
};

