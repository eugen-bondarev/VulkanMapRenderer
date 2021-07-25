#pragma once

#include "ecs/ecs.h"
#include "components/map/map.h"
#include "components/map_renderer/map_renderer.h"
#include "components/camera/camera.h"

#include "components/ui_renderer/ui_renderer.h"

class Game
{
public:
	Game();
	~Game();

	Entity* mapEntity;
	Map* map;
	MapRenderer* mapRenderer;

	Camera* camera;

	Entity* uiEntity;
	UIRenderer* uiRenderer;

private:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
};