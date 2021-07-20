#include "tiles.h"

bool TopBlockIsNot(const Blocks_t &blocks, int x, int y, BlockType type)
{
	return y > 0 && blocks[x][y - 1].type != type;
}

bool LeftBlockIsNot(const Blocks_t &blocks, int x, int y, BlockType type)
{
	return x > 0 && blocks[x - 1][y].type != type;
}

bool BottomBlockIsNot(const Blocks_t &blocks, int x, int y, BlockType type)
{
	return y < blocks[x].size() - 1 && blocks[x][y + 1].type != type;
}

bool RightBlockIsNot(const Blocks_t &blocks, int x, int y, BlockType type)
{
	return x < blocks.size() - 1 && blocks[x + 1][y].type != type;
}

#define CHECK_NEIGHBOURS(TOP, LEFT, BOTTOM, RIGHT) \
	if (TOP() && LEFT() && BOTTOM() && RIGHT())    \
		return glm::vec2{1, 2};                         \
	if (LEFT() && TOP() && RIGHT())                \
		return glm::vec2{-1, 3};                        \
	if (LEFT() && BOTTOM() && RIGHT())             \
		return glm::vec2{-1, 4};                        \
	if (TOP() && RIGHT() && BOTTOM())              \
		return glm::vec2{0, 3};                         \
	if (TOP() && LEFT() && BOTTOM())               \
		return glm::vec2{0, 4};                         \
	if (TOP() && LEFT())                           \
		return glm::vec2{-1, -1};                       \
	if (TOP() && RIGHT())                          \
		return glm::vec2{1, -1};                        \
	if (BOTTOM() && LEFT())                        \
		return glm::vec2{-1, 1};                        \
	if (BOTTOM() && RIGHT())                       \
		return glm::vec2{1, 1};                         \
	if (TOP() && BOTTOM())                         \
		return glm::vec2{0, 2};                         \
	if (LEFT() && RIGHT())                         \
		return glm::vec2{-1, 2};                        \
	if (TOP())                                     \
		return glm::vec2{0, -1};                        \
	if (BOTTOM())                                  \
		return glm::vec2{0, 1};                         \
	if (LEFT())                                    \
		return glm::vec2{-1, 0};                        \
	if (RIGHT())                                   \
		return glm::vec2{1, 0};                         \
	return glm::vec2 { 0, 0 }

glm::vec2 GrassGenerator(const Blocks_t &blocks, int x, int y)
{
#undef TOP
#undef LEFT
#undef BOTTOM
#undef RIGHT

#define TOP() TopBlockIsNot(blocks, x, y, blocks[x][y].type) && TopBlockIsNot(blocks, x, y, BlockType::Dirt)
#define LEFT() LeftBlockIsNot(blocks, x, y, blocks[x][y].type) && LeftBlockIsNot(blocks, x, y, BlockType::Dirt)
#define BOTTOM() BottomBlockIsNot(blocks, x, y, blocks[x][y].type) && BottomBlockIsNot(blocks, x, y, BlockType::Dirt)
#define RIGHT() RightBlockIsNot(blocks, x, y, blocks[x][y].type) && RightBlockIsNot(blocks, x, y, BlockType::Dirt)

	CHECK_NEIGHBOURS(TOP, LEFT, BOTTOM, RIGHT);
}

glm::vec2 DirtGenerator(const Blocks_t &blocks, int x, int y)
{
#undef TOP
#undef LEFT
#undef BOTTOM
#undef RIGHT

#define TOP() TopBlockIsNot(blocks, x, y, blocks[x][y].type) && TopBlockIsNot(blocks, x, y, BlockType::Grass)
#define LEFT() LeftBlockIsNot(blocks, x, y, blocks[x][y].type) && LeftBlockIsNot(blocks, x, y, BlockType::Grass)
#define BOTTOM() BottomBlockIsNot(blocks, x, y, blocks[x][y].type) && BottomBlockIsNot(blocks, x, y, BlockType::Grass)
#define RIGHT() RightBlockIsNot(blocks, x, y, blocks[x][y].type) && RightBlockIsNot(blocks, x, y, BlockType::Grass)

	CHECK_NEIGHBOURS(TOP, LEFT, BOTTOM, RIGHT);
}

glm::vec2 GrassGenerator1(BlockType up, BlockType down, BlockType left, BlockType right, int x, int y)
{
	return glm::vec2(0);
}

glm::vec2 DirtGenerator1(BlockType up, BlockType down, BlockType left, BlockType right, int x, int y)
{
	return glm::vec2(0);
}

glm::vec2 PickRightAngularBlock(const Blocks_t &blocks, int x, int y)
{
#undef TOP
#undef LEFT
#undef BOTTOM
#undef RIGHT

#define TOP() TopBlockIsNot(blocks, x, y, blocks[x][y].type)
#define LEFT() LeftBlockIsNot(blocks, x, y, blocks[x][y].type)
#define BOTTOM() BottomBlockIsNot(blocks, x, y, blocks[x][y].type)
#define RIGHT() RightBlockIsNot(blocks, x, y, blocks[x][y].type)

	CHECK_NEIGHBOURS(TOP, LEFT, BOTTOM, RIGHT);
}

glm::vec2 PickRightAngularWall(const Blocks_t &blocks, int x, int y)
{
#undef TOP
#undef LEFT
#undef BOTTOM
#undef RIGHT

#define TOP() !TopBlockIsNot(blocks, x, y)
#define LEFT() !LeftBlockIsNot(blocks, x, y)
#define BOTTOM() !BottomBlockIsNot(blocks, x, y)
#define RIGHT() !RightBlockIsNot(blocks, x, y)

	CHECK_NEIGHBOURS(TOP, LEFT, BOTTOM, RIGHT);
}

glm::vec2 PickRightAngularBlock1(BlockType up, BlockType down, BlockType left, BlockType right, int x, int y)
{
	return glm::vec2(0);
}

glm::vec2 PickRightAngularWall1(BlockType up, BlockType down, BlockType left, BlockType right, int x, int y)
{
	return glm::vec2(0);
}

TileFunction PickTileFunction(TileType type)
{
	bool special = tileFunctions.find(type) != tileFunctions.end();
	return special ? tileFunctions[type] : PickRightAngularBlock;
}

TileFunction1 PickTileFunction1(TileType type)
{
	bool special = tileFunctions1.find(type) != tileFunctions1.end();
	return special ? tileFunctions1[type] : PickRightAngularBlock1;
}