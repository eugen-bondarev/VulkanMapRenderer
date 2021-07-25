#include "game.h"

Game::Game()
{
	mapEntity = new Entity();

	camera = AddComponent<Camera>(mapEntity);
	map = AddComponent<Map>(mapEntity);
	mapRenderer = AddComponent<MapRenderer>(mapEntity);

	uiEntity = new Entity();
	uiRenderer = AddComponent<UIRenderer>(uiEntity);
}

Game::~Game()
{
	delete uiEntity;
	delete mapEntity;
}