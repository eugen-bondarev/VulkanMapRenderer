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
};

template <typename T, typename... Args>
T* AddComponent(Entity* entity, Args&&... args)
{
	T* component = new T(std::forward<Args>(args)...);
	entity->components.push_back(component);
	
	Collections::AddToCollection(component);

	component->entity = entity;
	return component;
}

template <typename T>
T* GetComponent(Entity* entity)
{
	for (int i = 0; i < entity->components.size(); i++)
		if (T* component = dynamic_cast<T*>(entity->components[i]))
			return component;

	return nullptr;
}

template <typename T>
void RemoveComponent(Entity* entity)
{
	T* component = GetComponent<T>(entity);

	if (component)
	{
		for (int i = 0; i < entity->components.size(); i++)
		{
			if (entity->components[i] == component)
			{
				// Removing from entity's component list
				{
					Engine::Util::Vector::RemoveAt(entity->components, i);
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