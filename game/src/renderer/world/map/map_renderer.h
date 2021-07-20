#pragma once

#include "game/world/map/map.h"

namespace MapRenderer
{
	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data);
}