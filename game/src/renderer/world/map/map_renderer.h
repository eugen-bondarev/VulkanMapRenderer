#pragma once

#include "game/world/map/map.h"
#include "game/game.h"
#include "renderer/atlas/tile_map.h"

#include "color_pass.h"
#include "composition.h"

namespace MapRenderer
{
	extern Composition* composition;

	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data);
}