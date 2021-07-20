#pragma once

#include "tile_map.h"

#include <memory>

enum class TextureAtlasType
{
	Map,
	Tools,
	InterfaceIcons
};

class TextureAtlas
{
public:
	template <typename T>
	static std::shared_ptr<T> Add(TextureAtlasType type, std::shared_ptr<T> texture)
	{
		dictionary[type] = dynamic_cast<TileMap*>(texture.get());
		return texture;
	}

	template <typename T>
	static T *Get(TextureAtlasType type)
	{
		return static_cast<T *>(dictionary[type]);
	}

	static void Clear()
	{
		dictionary.clear();
	}

private:
	inline static std::map<TextureAtlasType, TileMap *> dictionary;
};