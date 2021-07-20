#include "tile_map.h"

glm::vec2 TileMap::GetAmountOfTiles() const
{
	return amountOfTiles;
}

glm::vec2 TileMap::GetTileSize() const
{
	return size / amountOfTiles;
}