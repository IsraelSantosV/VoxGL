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
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		inline static bool IsKeyPressed(KeyCode key) { return m_Instance->IsKeyPressedImpl(key); }

		inline static bool IsMouseButtonPressed(MouseCode button) { return m_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return m_Instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return m_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return m_Instance->GetMouseYImpl(); }

		static Scope<Input> Create();

	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;

		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Scope<Input> m_Instance;
	};
}