#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>

#include "factory.h"

namespace ecs
{
	struct Entity;
	struct Component;

	struct Component
	{
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

	template <typename T>
	T* AddComponent(Entity* entity)
	{
		T* component = new T();
		entity->components.push_back(component);
		return component;
	}

	Component* AddComponent(Entity* entity, const std::string& name);

	class MAP : public Component
	{
	public:
		float foobar;

		MAP()
		{

		}

		~MAP()
		{

		}
	};
	REGISTER(MAP);
}