#pragma once

#include "ecs/camera.h"
	
#include "../ecs/ecs.h"
#include "../components/map/map.h"
#include "../components/map_renderer/map_renderer.h"

class Game
{
public:
	Game();
	~Game();

	Entity* mapEntity;
	Map* map;
	MapRenderer* mapRenderer;

	Camera camera;

private:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
};