#include "game.h"

Game::Game()
{
	// map = std::make_shared<Map>();

	mapEntity = new Entity();

	map = AddComponent<Map>(mapEntity);
	mapRenderer = AddComponent<MapRenderer>(mapEntity);
	camera = AddComponent<Camera>(mapEntity);
}

Game::~Game()
{
	delete mapEntity;
}