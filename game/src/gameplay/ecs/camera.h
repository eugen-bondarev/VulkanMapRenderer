#pragma once

#include "transform.h"

#include <engine/common.h>

namespace Gameplay
{
	enum CameraEvents_
	{
		CameraEvents_None = 0,
		CameraEvents_PositionChanged = 1 << 0,
		CameraEvents_Initial = CameraEvents_PositionChanged
	};
	ENUM_USE_FOR_FLAGS(CameraEvents_)

	class Camera : public ITransform
	{
	public:
		void CalculateTransformMatrix() override;

		CameraEvents_& GetEvents();

		void CheckPositionChange();

	private:
		glm::vec2 lastPosition { -1 };

		CameraEvents_ events;
	};
}