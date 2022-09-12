#pragma once

#include "Vox/Core/Layer.h"

#include "Vox/Events/AplicationEvent.h"
#include "Vox/Events/KeyEvent.h"
#include "Vox/Events/MouseEvent.h"

namespace Vox
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void SetThemeColors();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void Begin();
		void End();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
