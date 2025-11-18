#include "pch.h"
#include "main.h"
#include <LightEngine/GameManager.h>
#include "TestScene.h"

int main()
{
	GameManager* pInstance = GameManager::Get();

	pInstance->Init(1280, 720);

	pInstance->LaunchScene<TestScene>();

	return 0;
}
