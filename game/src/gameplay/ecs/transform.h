#pragma once

#include <glm/glm.hpp>

namespace Gameplay
{
	class ITransform
	{
	public:
		template <typename... Args>
		void SetPosition(Args&&... args)
		{
			position = glm::vec2(std::forward<Args>(args)...);
			CalculateTransformMatrix();
		}
		
		template <typename... Args>
		void AddPosition(Args&&... args)
		{
			position += glm::vec2(std::forward<Args>(args)...);
			CalculateTransformMatrix();
		}

		glm::vec2 GetPosition() const;
		const glm::mat4x4& GetTransformMatrix() const;

		virtual void CalculateTransformMatrix() = 0;

	protected:
		glm::vec2 position;
		glm::mat4x4 transformMatrix;
	};
}