#pragma once

#include "GameManager.h"
#include "Scene.h"

template<typename T>
void GameManager::LaunchScene()
{
	static_assert(std::is_base_of<Scene, T>::value, "T must be derived from Scene");
	_ASSERT(m_pScene == nullptr);

	T* newScene = new T();
	m_pScene = newScene;

	m_pScene->SetGameManager(this);
	m_pScene->OnInitialize();

	Run();
}
