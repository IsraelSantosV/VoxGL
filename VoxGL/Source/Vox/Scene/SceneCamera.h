#pragma once

#include "Vox/Renderer/Camera.h"

namespace Vox
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicSize() const { return m_OrthoSize; }
		void SetOrthographicSize(float size)
		{
			m_OrthoSize = size;
			RecalculateProjection();
		}
	private:
		void RecalculateProjection();
	private:
		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;
		float m_AspectRatio = 0.0f;
	};
}