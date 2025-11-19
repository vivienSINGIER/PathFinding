#pragma once

#include "3DLightEngine/Scene.h"
#include "3DLightEngine/GameManager.h"
#include "3DLightEngine/Entity.h"

template<typename T>
T* Scene::CreateEntity(float radius, const gce::Vector3f32 color)
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	T* newEntity = new T();

	Entity* entity = newEntity;
	entity->Initialize(radius, color);
	
	m_pGameManager->mEntitiesToAdd.push_back(newEntity);

	return newEntity;
}
