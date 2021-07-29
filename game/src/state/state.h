#pragma once

#include "ecs/ecs.h"

#include "components/camera/camera.h"
#include "components/map/map.h"
#include "components/map_renderer/map_renderer.h"

#include "components/ui_renderer/ui_renderer.h"

#include "components/menu_renderer/menu_renderer.h"

class State
{
public:
	State();
	~State();

	Entity* mapEntity;
	Entity* uiEntity;

	Camera* camera;
	Map* map;
	MapRenderer* mapRenderer;

	UIRenderer* uiRenderer;

	Entity* menuEntity;
	MenuRenderer* menuRenderer;

private:
	State(const State&) = delete;
	State& operator=(const State&) = delete;
};