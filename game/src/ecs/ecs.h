#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>

#include "factory.h"

#include "component.h"
#include "collections.h"

struct Entity
{
	std::vector<Component*> components;

	Entity()
	{

	}
	
	virtual ~Entity()
	{
		for (auto& component : components)
			delete component;
	}

	template <typename T, typename... Args>
	T* AddComponent(Args&&... args)
	{
		T* component = new T(std::forward<Args>(args)...);
		components.push_back(component);
		
		Collections::AddToCollection(component);

		component->entity = this;
		return component;
	}

	template <typename T>
	T* GetComponent()
	{
		for (int i = 0; i < components.size(); i++)
			if (T* component = dynamic_cast<T*>(components[i]))
				return component;

		return nullptr;
	}

	template <typename T>
	void RemoveComponent()
	{
		T* component = GetComponent<T>(entity);

		if (component)
		{
			for (int i = 0; i < components.size(); i++)
			{
				if (components[i] == component)
				{
					// Removing from entity's component list
					{
						Engine::Util::Vector::RemoveAt(components, i);
					}

					// Removing from a collection (if there is one containing it)
					{
						Collections::RemoveFromCollection(component);
					}

					{
						delete component;
					}
					
					break;
				}
			}
		}
	}
};
