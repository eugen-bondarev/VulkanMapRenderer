#pragma once

struct Entity;

struct Component
{
	Entity* entity;

	Component();
	virtual ~Component();
};