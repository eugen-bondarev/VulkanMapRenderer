#pragma once

#include "ecs/ecs.h"

#include <engine/engine.h>

#include "game/world/map/blocks.h"

#include "FastNoiseLite/FastNoiseLite.h"

class Map : public Component
{
friend class MapRenderer;

public:
	inline static constexpr float BLOCK_SIZE = 16.0f;

	/*
	* These are chunks, but we can go without them by setting CHUNK_SIZE = 1, which I actually do.
	*/
	inline static constexpr float CHUNK_SIZE = 1.0f;

	struct VisibleChunks
	{
		glm::ivec2 start;
		glm::ivec2 end;
	} visibleChunks, lastVisibleChunks;

public:
	Map();
	~Map();

	void CalculateVisibleChunks(glm::vec2 view_position);

	/**
	 * Note:
	 * 	Make sure to use CalculateVisibleChunks(camera.GetPosition());
	 * 	before calling this method.
	 */
	void PopulateBlocks();
	void Async_PopulateBlocks(int start, int end);

	Blocks_t& GetBlocks();
	int GetAmountOfBlocks() const;

	bool WithinTopBoundsX(int x) const;
	bool WithinTopBoundsY(int y) const;
	bool WithinTopBounds(glm::vec2 indices) const;

	bool WithinBottomBoundsX(int x) const;
	bool WithinBottomBoundsY(int y) const;
	bool WithinBottomBounds(glm::vec2 indices) const;
	
private:
	Blocks_t blocks;
	std::vector<glm::vec4> blocksForRendering;

	FastNoiseLite noise;

	void DetermineDimensionsInBlocks();

	Map(const Map&) = delete;	
	Map& operator=(const Map&) = delete;	
};