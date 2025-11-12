#include "Agent.h"
#include "Grid.h"

void Agent::OnUpdate()
{
	Grid* grid = GetScene<Grid>();
	if (grid == nullptr) return;

	sf::Vector2f tempPos = { mShape.getPosition().x, mShape.getPosition().y };
	sf::Vector2i worldPos = { (int)(tempPos.x), (int)(tempPos.y) };
	m_tilePosition = grid->GetTilePosition(worldPos);

	Node<Tile>* tempNode = grid->GetNode(m_tilePosition);
	if (tempNode == nullptr) return;

	tempNode->data->isOccupied = true;
}

void Agent::OnInitialize()
{

}