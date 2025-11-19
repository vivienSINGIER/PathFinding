#include "3DLightEngine/Entity.h"
#include "3DLightEngine/GameManager.h"

template<typename T>
T* Entity::GetScene() const
{
	T* pScene = dynamic_cast<T*>(GetScene());

	_ASSERT(pScene != nullptr);

	return pScene;
}


template<typename T>
T* Entity::CreateEntity(float radius, gce::Vector3f32 color)
{
	return GetScene()->CreateEntity<T>(radius, color);
}