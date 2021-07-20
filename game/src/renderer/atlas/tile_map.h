#pragma once

#include <engine/core/graphics/vk/image/texture2d.h>

#include <map>

#include "game/world/map/blocks.h"

class TileMap : public Engine::Vk::Texture2D
{
public:
	template <typename... Args>
	TileMap(glm::vec2 tile_size, Args &&...args) : Engine::Vk::Texture2D(std::forward<Args>(args)...)
	{
		amountOfTiles = size / tile_size;
	}

	glm::vec2 GetAmountOfTiles() const;
	glm::vec2 GetTileSize() const;

private:
	glm::vec2 amountOfTiles;
};

template <typename T>
class ITileMapDictionary
{
public:
	template <typename... Args>
	void Setup(Args &&...args)
	{
		dictionary = {std::forward<Args>(args)...};
	}

	void Add(T t, glm::vec2 vec)
	{
		dictionary[t] = vec;
	}

	glm::vec2 Get(T t) const
	{
		return dictionary.at(t);
	}

public:
	std::map<T, glm::vec2> dictionary;
};

class BlocksTileMap : public TileMap, public ITileMapDictionary<BlockType>
{
public:
	template <typename... Args>
	BlocksTileMap(Args &&...args) : TileMap(std::forward<Args>(args)...) {}
};