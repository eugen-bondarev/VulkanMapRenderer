#pragma once

#include "../../../world/map/map.h"

namespace Game
{
	namespace MapRenderer
	{
		std::vector<glm::vec4> GetRenderData(Map* map, glm::vec2 view_position);
	}
}