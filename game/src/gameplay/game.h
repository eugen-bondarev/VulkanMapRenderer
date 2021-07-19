#pragma once

#include "../world/map/map.h"
#include "../renderer/world/map/map_renderer.h"
#include "ecs/camera.h"

namespace Gameplay
{
	class Game
	{
	public:
		Game() = default;

		Camera camera;
		Map map;

	private:
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;
	};
}
