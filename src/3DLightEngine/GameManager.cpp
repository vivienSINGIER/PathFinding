#include "3DLightEngine/GameManager.h"

#include "3DLightEngine/Entity.h"
#include "3DLightEngine/Debug.h"
#include "3DLightEngine/Scene.h"
#include "GC-simple-render/InputsMethods.h"

#include <iostream>

GameManager::GameManager()
{
	m_pWindow = nullptr;
	m_pCamera = nullptr;
	m_DeltaTime = 0.0f;
	m_pScene = nullptr;
	m_WindowWidth = -1;
	m_WindowHeight = -1;
}

GameManager* GameManager::Get()
{
	static GameManager mInstance;

	return &mInstance;
}

GameManager::~GameManager()
{
	delete m_pWindow;
	delete m_pScene;

	for (Entity* entity : mEntities)
	{
		delete entity;
	}
}

void GameManager::Init(unsigned int width, unsigned int height)
{
	_ASSERT(m_pWindow == nullptr);

	m_pWindow = new Window(L"Game Window", width, height);
	m_pCamera = new Camera(CameraType::PERSPECTIVE);
	m_pCamera->SetPosition({0.0f, 20.0f, 0.0f});
	m_pCamera->SetRotation({45.0f, 0.0f, 0.0f});
	m_pCamera->SetFOV(gce::PI/4.0f);
	m_pCamera->SetFarPlane(500.0f);
	m_pCamera->SetNearPlane(0.001f);

	m_WindowWidth = width;
	m_WindowHeight = height;

	Debug::Get()->Init();
}

void GameManager::Run()
{
	if (m_pWindow == nullptr) 
	{
		std::cout << "Window not created, creating default window" << std::endl;
		Init(1280, 720);
	}

	_ASSERT(m_pScene != nullptr);

	while (m_pWindow->IsOpen())
	{
		SetDeltaTime(m_Chrono.Reset());

		HandleInput();

		Update();
		
		Draw();
	}
}

void GameManager::HandleInput()
{
	if (m_pScene == nullptr) return;

	m_pScene->HandleInput();
}

void GameManager::Update()
{
	m_pScene->OnUpdate();

    //Update
    for (auto it = mEntities.begin(); it != mEntities.end(); )
    {
		Entity* entity = *it;

        if (entity->IsActive() == false)
		{
			++it;
			continue;
		}

		entity->Update();

        if (entity->ToDestroy() == false)
        {
            ++it;
            continue;
        }

		mEntitiesToDestroy.push_back(entity);
		++it;
        //it = mEntities.erase(it);
    }

    //Collision
    for (auto it1 = mEntities.begin(); it1 != mEntities.end(); ++it1)
    {
        auto it2 = it1;
        ++it2;
        for (; it2 != mEntities.end(); ++it2)
        {
            Entity* entity = *it1;
            Entity* otherEntity = *it2;

            if (entity->IsColliding(otherEntity))
            {
				if (entity->IsRigidBody() && otherEntity->IsRigidBody())
					entity->Repulse(otherEntity);

                entity->OnCollision(otherEntity);
                otherEntity->OnCollision(entity);
            }
        }
    }

	//Destruction des entités
	for (auto it = mEntitiesToDestroy.begin(); it != mEntitiesToDestroy.end(); ++it) 
	{
		Entity* entity = *it;
		entity->SetActive(false);
		/*delete *it;*/
	}

    mEntitiesToDestroy.clear();

	//Ajout des entités
	for (auto itAdd = mEntitiesToAdd.begin(); itAdd != mEntitiesToAdd.end(); ++itAdd)
	{
		auto itReplace = mEntities.end();
		for (auto it = mEntities.begin(); it != mEntities.end(); ++it)
		{
			Entity* entityToCheck = *it;
			if (entityToCheck->IsActive() == false)
			{
				itReplace = it;
				break;
			}
		}

		if (itReplace != mEntities.end())
		{
			*itReplace = *itAdd;
		}
		else
			mEntities.push_back(*itAdd);
	}

	mEntitiesToAdd.clear();
}

void GameManager::Draw()
{
	Debug::Get()->Draw(*m_pWindow);

	m_pWindow->Begin(*m_pCamera);
	
	for (Entity* entity : mEntities)
	{
		if (entity->IsActive())
			m_pWindow->Draw(*entity->GetShape());
	}

	m_pWindow->End();
	m_pWindow->Display();

	Debug::Get()->Reset();
}
