#pragma once

#include <list>

#include "GC-simple-render/Camera.h"
#include "GC-simple-render/Window.h"
#include "Chrono.h"

class Entity;
class Scene;
class Debug;

class GameManager
{
	std::list<Entity*> mEntities;
	std::list<Entity*> mEntitiesToDestroy;
	std::list<Entity*> mEntitiesToAdd;

	Window* m_pWindow;

	Camera* m_pCamera;

	Scene* m_pScene;

	Chrono m_Chrono;

	float m_DeltaTime;

	int m_WindowWidth;
	int m_WindowHeight;

private:
	GameManager();

	void Run();
	
	void HandleInput();
	void Update();
	void Draw();

	void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }


public:
	~GameManager();
	static GameManager* Get();

	void Init(unsigned int width, unsigned int height);

	template<typename T>
	void LaunchScene();

	float GetDeltaTime() const { return m_DeltaTime; }
	Scene* GetScene() const { return m_pScene; }
	Window* GetWindow() const { return m_pWindow; }
	Camera* GetCamera() const { return m_pCamera; }

	friend Debug;
	friend Scene;
};

#include "GameManager.inl"