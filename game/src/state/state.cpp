#include "state.h"

State::State()
{
	// mapEntity = new Entity();
	// uiEntity = new Entity();
	
	// camera = mapEntity->AddComponent<Camera>();
	// map = mapEntity->AddComponent<Map>(camera);
	// mapRenderer = mapEntity->AddComponent<MapRenderer>(camera, map);

	// uiRenderer = uiEntity->AddComponent<UIRenderer>();

	menuEntity = new Entity();
	MenuRenderer* menuRenderer = menuEntity->AddComponent<MenuRenderer>();
}

State::~State()
{
	delete uiEntity;
	delete mapEntity;
}