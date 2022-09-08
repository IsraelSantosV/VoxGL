#pragma once

#include "Vox/Core/Core.h"
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
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}