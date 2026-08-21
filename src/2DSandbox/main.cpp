
#include <SFML/Graphics.hpp>
#include <iostream>

#include "LightEngine/GameManager.h"
#include "Grid.h"

#include <cstdlib>
#include <crtdbg.h>

int main() 
{
    GameManager* pInstance = GameManager::Get();

	pInstance->CreateGameWindow(1920, 1080, "SampleScene", 60, sf::Color::Black);
	
	pInstance->LaunchScene<Grid>();

	return 0;
}