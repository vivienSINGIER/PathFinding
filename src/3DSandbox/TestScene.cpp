#include "pch.h"
#include "TestScene.h"
#include <Maths/Vector3.h>

#include "Debug.h"
#include "GC-simple-render/InputsMethods.h"

void TestScene::OnInitialize()
{
	m_pEntity1 = CreateEntity<Entity>(1, gce::Vector3f32 {0.7f, 0.7f, 0.f});
	m_pEntity1->SetPosition(5.f, 0.f, 0.f);
	m_pEntity1->SetRigidBody(true);
	
	// m_pEntity2 = CreateEntity<Entity>(1, gce::Vector3f32{ 0.f, 0.7f, 0.7f });
	// m_pEntity2->SetPosition(-5.f, 0.2f, 0.f);
	// m_pEntity2->SetRigidBody(true);
}

void TestScene::OnUpdate()
{
	gce::Vector2i8 startPos = {0, -0};
	
	
	if (GetKeyDown(Keyboard::ENTER))
	{
		m_pEntity1->GoToPosition(-m_pEntity1->GetPosition().x, 0.0f, 0.0f, 5.0f);
		// m_pEntity2->GoToPosition(-m_pEntity2->GetPosition().x, 0.2f, 0.0f, 5.0f);
	}
}
