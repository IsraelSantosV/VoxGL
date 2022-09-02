#pragma once

#include "Vox/Core.h"

namespace Vox
{
	class VOX_API Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) 
		{ 
			return m_Instance->IsKeyPressedImpl(keycode); 
		}

		inline static bool IsMouseButtonPressed(int button)
		{
			return m_Instance->IsMouseButtonPressedImpl(button);
		}

		inline static std::pair<float, float> GetMousePosition() { return m_Instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return m_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return m_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
	private:
		static Input* m_Instance;
	};
}