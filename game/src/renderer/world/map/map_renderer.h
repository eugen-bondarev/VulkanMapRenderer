#pragma once

#include "../../../world/map/map.h"

namespace Gameplay
{
	namespace MapRenderer
	{
		std::vector<glm::vec4> GetRenderData(Map* map, glm::vec2 view_position);
	}
}