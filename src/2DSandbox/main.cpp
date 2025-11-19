
#include <SFML/Graphics.hpp>
#include <iostream>

#include "LightEngine/GameManager.h"
#include "Grid.h"

#include <cstdlib>
#include <crtdbg.h>

int main() 
{
    GameManager* pInstance = GameManager::Get();

	pInstance->CreateWindow(1280, 720, "SampleScene", 60, sf::Color::Black);
	
	pInstance->LaunchScene<Grid>();

	return 0;
}