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
	inline static constexpr float CHUNK_SIZE = 4.0f;

	struct VisibleBlocks
	{
		glm::ivec2 start;
		glm::ivec2 end;
	} visibleBlocks, lastVisibleBlocks;

public:
	Map();
	~Map();

	Blocks_t& GetBlocks();

	void Async_PopulateBlocks(int start, int end);
	void PopulateBlocks(glm::vec2 view_position);

	void CalculateVisibleBlocks(glm::vec2 view_position);

	int GetAmountOfBlocks() const;
	
private:
	Blocks_t blocks;
	std::vector<glm::vec4> blocksForRendering;

	FastNoiseLite noise;

	void DetermineDimensionsInBlocks();

	Map(const Map&) = delete;	
	Map& operator=(const Map&) = delete;	
};