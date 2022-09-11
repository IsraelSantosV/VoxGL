#pragma once

#include "Vox.h"

namespace Vox
{
	class CameraController : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override
		{
			auto& transform = GetComponent<TransformComponent>().Transform;
			transform[3][0] = rand() % 10 - 5.0f;
		}

		virtual void OnDestroy() override
		{
		}

		virtual void OnUpdate(Timestep ts) override
		{
			auto& transform = GetComponent<TransformComponent>().Transform;

			float speed = 5.0f;

			if (Input::IsKeyPressed(Key::A))
				transform[3][0] -= speed * ts;
			if (Input::IsKeyPressed(Key::D))
				transform[3][0] += speed * ts;
			if (Input::IsKeyPressed(Key::W))
				transform[3][1] += speed * ts;
			if (Input::IsKeyPressed(Key::S))
				transform[3][1] -= speed * ts;
		}
	};
}
