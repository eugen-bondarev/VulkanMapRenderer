#pragma once

#include <engine/engine.h>

#include "component.h"

#include <vector>

namespace Collections
{
	template <typename T>
	class Collection
	{
	public:
		inline static std::vector<T*> collection;
	};

	class IRenderable : public Collection<IRenderable>
	{
	public:
		virtual void Render(Engine::Vk::Frame* frame) = 0;

		inline static void RenderAll(Engine::Vk::Frame* frame)
		{
			for (auto& element : collection)
				element->Render(frame);
		}
	};

	class IOnUpdate : public Collection<IOnUpdate>
	{
	public:
		virtual void Update() = 0;

		inline static void UpdateAll()
		{
			for (auto& element : collection)
				element->Update();
		}
	};

	void AddToCollection(Component* element);
	void RemoveFromCollection(Component* element);
}