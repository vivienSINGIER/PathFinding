#include "main.h"
#include <3DLightEngine/GameManager.h>
#include "TestScene.h"

int main()
{
	GameManager* pInstance = GameManager::Get();

	pInstance->Init(1280, 720);

	pInstance->LaunchScene<TestScene>();

	return 0;
}
