#pragma once

#include "world/map/map.h"
#include "ecs/camera.h"

class Game
{
public:
	Game();

	Camera camera;
	std::shared_ptr<Map> map;

private:
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
};