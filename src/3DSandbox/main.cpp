#include "pch.h"
#include "main.h"
#include <3DLightEngine/GameManager.h>
#include "3DSandbox/Grid3D.h"

int main()
{
	GameManager* pInstance = GameManager::Get();

	pInstance->Init(1280, 720);

	pInstance->LaunchScene<Grid3D>();

	return 0;
}
