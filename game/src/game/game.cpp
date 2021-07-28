#include "game.h"

Game::Game()
{
	mapEntity = new Entity();

	camera = mapEntity->AddComponent<Camera>();
	map = mapEntity->AddComponent<Map>(camera);
	mapRenderer = mapEntity->AddComponent<MapRenderer>(camera, map);

	uiEntity = new Entity();
	uiRenderer = uiEntity->AddComponent<UIRenderer>();
}

Game::~Game()
{
	delete uiEntity;
	delete mapEntity;
}