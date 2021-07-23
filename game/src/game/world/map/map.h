#pragma once

#include <engine/engine.h>

#include "blocks.h"

#include "FastNoiseLite/FastNoiseLite.h"

struct Settings
{
	float size0 = 0.3f;
	float size1 = 0.001f;
	float size2 = 0.1f;
	float bias0 = 0.0f;
	float bias1 = 0.0f;
};

inline static Settings settings;

class Map
{
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
	
private:
	Blocks_t blocks;
	std::vector<glm::vec4> blocksForRendering;

	FastNoiseLite noise;

	void DetermineDimensionsInBlocks();

	Map(const Map&) = delete;	
	Map& operator=(const Map&) = delete;	
};