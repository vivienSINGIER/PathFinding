#include "pch.h"
#include "TestScene.h"
#include <Maths/Vector3.h>

void TestScene::OnInitialize()
{
	m_pEntity1 = CreateEntity<Entity>(100, gce::Vector3f32 {0.f, 0.f, 0.f});
	m_pEntity1->SetPosition(100.f, 0.f, 100.f);
	m_pEntity1->SetRigidBody(true);

	m_pEntity2 = CreateEntity<Entity>(50, gce::Vector3f32{ 0.f, 0.f, 0.f });
	m_pEntity2->SetPosition(500.f, 0.f, 500.f);
	m_pEntity2->SetRigidBody(true);
}

void TestScene::OnUpdate()
{
}
