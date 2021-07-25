#include "game.h"

Game::Game()
{
	// map = std::make_shared<Map>();

	mapEntity = new Entity();

	camera = AddComponent<Camera>(mapEntity);
	map = AddComponent<Map>(mapEntity);
	mapRenderer = AddComponent<MapRenderer>(mapEntity);
}

Game::~Game()
{
	delete mapEntity;
}