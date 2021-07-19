#pragma once

#include <engine/engine.h>

#include "blocks.h"

#include "FastNoiseLite/FastNoiseLite.h"

namespace Gameplay
{
	class Map
	{
	public:
		inline static constexpr float BLOCK_SIZE = 16.0f;
		inline static constexpr float CHUNK_SIZE = 2.0f;

		struct VisibleBlocks
		{
			glm::ivec2 start;
			glm::ivec2 end;
		} visibleBlocks, lastVisibleBlocks;

		struct Settings
		{
			float size0 = 0.3f;
			float size1 = 0.001f;
			float size2 = 0.1f;
			float bias0 = 0.0f;
			float bias1 = 0.0f;
		} settings;

	public:
		Map();
		~Map();

		FastNoiseLite noise;

		Blocks_t& GetBlocks();

		void PopulateBlocks(glm::vec2 view_position);
		void CalculateVisibleBlocks(glm::vec2 view_position);
		
	private:
		Blocks_t blocks;
		std::vector<glm::vec4> blocksForRendering;
	
		void DetermineDimensionsInBlocks();

		Map(const Map&) = delete;	
		Map& operator=(const Map&) = delete;	
	};
}