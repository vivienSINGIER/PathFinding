#pragma once
#include <Maths/Vector3.h>

class GameManager;

class Scene
{
private:
	GameManager* m_pGameManager;

private:
	void SetGameManager(GameManager* pGameManager) { m_pGameManager = pGameManager; }
	
protected:
	Scene() = default;

	virtual void OnInitialize() = 0;
	/*virtual void OnEvent(const sf::Event& event) = 0;*/
	virtual void OnUpdate() = 0;
	virtual void HandleInput() = 0;

public:
	template<typename T>
	T* CreateEntity(float radius, gce::Vector3f32 color);

	float GetDeltaTime() const;

	int GetWindowWidth() const;
	int GetWindowHeight() const;

	friend GameManager;
};

#include "Scene.inl"