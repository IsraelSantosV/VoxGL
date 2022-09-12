#pragma once

#include "Vox.h"

namespace Vox
{
	class CameraController : public ScriptableEntity
	{
	public:
		virtual void OnUpdate(Timestep ts) override
		{
			auto& position = GetComponent<TransformComponent>().Position;

			float speed = 5.0f;

			if (Input::IsKeyPressed(Key::A))
				position.x -= speed * ts;
			if (Input::IsKeyPressed(Key::D))
				position.x += speed * ts;
			if (Input::IsKeyPressed(Key::W))
				position.y += speed * ts;
			if (Input::IsKeyPressed(Key::S))
				position.y -= speed * ts;
		}
	};
}
