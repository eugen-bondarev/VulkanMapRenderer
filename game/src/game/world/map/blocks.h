#pragma once

#include <vector>

#include <glm/vec2.hpp>

enum class TileType
{
	Empty = 0,
	Dirt,
	Grass,
	Stone,
	Wood
};

enum class TilePos
{
	Background,
	Foreground
};

struct Tile
{
	TileType type;
	glm::vec2 worldPosition;
};

using BlockType = TileType;
using WallType = TileType;

using BlocksColumn_t = std::vector<Tile>;
using Blocks_t = std::vector<BlocksColumn_t>;

using WallsColumn_t = std::vector<Tile>;
using Walls_t = std::vector<WallsColumn_t>;
