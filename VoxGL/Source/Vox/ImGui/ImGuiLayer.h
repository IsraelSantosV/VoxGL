#pragma once

#include "Vox/Layer.h"

#include "Vox/Events/AplicationEvent.h"
#include "Vox/Events/KeyEvent.h"
#include "Vox/Events/MouseEvent.h"

namespace Vox
{
	class VOX_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}