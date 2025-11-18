#include "GameManager.h"

#include "Entity.h"
#include "Debug.h"
#include "Scene.h"

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

	m_WindowWidth = width;
	m_WindowHeight = height;
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
	//sf::Event event;
	//while (mpWindow->pollEvent(event))
	//{
	//	if (event.type == sf::Event::Closed)
	//	{
	//		mpWindow->close();
	//	}

	//	mpScene->OnEvent(event);
	//}
}

void GameManager::Update()
{
	m_pScene->OnUpdate();

    //Update
    for (auto it = mEntities.begin(); it != mEntities.end(); )
    {
		Entity* entity = *it;

        entity->Update();

        if (entity->ToDestroy() == false)
        {
            ++it;
            continue;
        }

        mEntitiesToDestroy.push_back(entity);
        it = mEntities.erase(it);
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

	for (auto it = mEntitiesToDestroy.begin(); it != mEntitiesToDestroy.end(); ++it) 
	{
		delete *it;
	}

    mEntitiesToDestroy.clear();

	for (auto it = mEntitiesToAdd.begin(); it != mEntitiesToAdd.end(); ++it)
	{
		mEntities.push_back(*it);
	}

	mEntitiesToAdd.clear();
}

void GameManager::Draw()
{
	//Debug::Get()->DrawBack(m_pWindow);

	for (Entity* entity : mEntities)
	{
		m_pWindow->Draw(*entity->GetShape());
	}
	
	//Debug::Get()->DrawFront(m_pWindow);

	m_pWindow->Display();
}
