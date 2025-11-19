#include "3DLightEngine/Scene.h"

#include "3DLightEngine/GameManager.h"

int Scene::GetWindowWidth() const
{
	return m_pGameManager->m_WindowWidth;
}

int Scene::GetWindowHeight() const
{
	return m_pGameManager->m_WindowHeight;
}

float Scene::GetDeltaTime() const
{
	return m_pGameManager->m_DeltaTime;
}
