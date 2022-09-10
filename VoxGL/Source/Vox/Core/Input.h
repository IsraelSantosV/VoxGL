#pragma once

#include <glm/glm.hpp>

#include "Vox/Core/KeyCodes.h"
#include "Vox/Core/MouseCodes.h"

namespace Vox
{
	class Input
	{
	protected:
		Input() = default;
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}