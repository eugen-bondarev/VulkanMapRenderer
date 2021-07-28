#pragma once

#include "transform.h"

#include <engine/common.h>

#include "ecs/ecs.h"

enum CameraEvents_
{
	CameraEvents_None = 0,
	CameraEvents_PositionChanged = 1 << 0,
	CameraEvents_Initial = CameraEvents_PositionChanged
};
ENUM_USE_FOR_FLAGS(CameraEvents_)

class Camera : public ITransform, public Component, public Collections::IOnUpdate
{
public:
	Camera();

	void Update() override;

	void CalculateTransformMatrix() override;
	void CalculateProjectionViewMatrix();

	CameraEvents_& GetEvents();

	void CheckPositionChange();

	const glm::mat4x4& GetProjectionViewMatrix() const;

private:
	glm::vec2 lastPosition { -1 };

	glm::mat4x4 projectionViewMatrix { 1 };

	CameraEvents_ events;
};