#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>

#include "factory.h"

struct Entity;
struct Component;

struct Component
{
	Entity* entity;

	Component()
	{

	}

	virtual ~Component()
	{

	}
};

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
	component->entity = entity;
	return component;
}

// Component* AddComponent(Entity* entity, const std::string& name);

template <typename T>
T* GetComponent(Entity* entity)
{
	for (int i = 0; i < entity->components.size(); i++)
		if (T* component = dynamic_cast<T*>(entity->components[i]))
			return component;

	return nullptr;
}