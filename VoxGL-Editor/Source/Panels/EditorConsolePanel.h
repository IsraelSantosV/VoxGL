#pragma once

#include "Vox/Core/Core.h"
#include "Vox/Editor/Console/ConsoleMessage.h"
#include "Vox/Renderer/Texture.h"
#include "EditorPanel.h"
#include "Vox/Events/Event.h"
#include "Vox/Events/SceneEvents.h"

#include <imgui/imgui.h>

namespace Vox
{
	class EditorConsolePanel : public EditorPanel
	{
	public:
		EditorConsolePanel();
		~EditorConsolePanel();

		void OnEvent(Event& event);
		virtual void OnRender() override;
	private:
		void TriggerEvent(ScenePreStartEvent& e);
		void RenderMenu(const ImVec2& size);
		void RenderConsole(const ImVec2& size);
		const char* GetMessageType(const ConsoleMessage& message) const;
		const ImVec4& GetMessageColor(const ConsoleMessage& message) const;
		ImVec4 GetToolbarButtonColor(const bool value) const;
	private:
		static void PushMessage(const ConsoleMessage& message);
	private:
		bool m_ClearOnPlay = true;

		std::vector<ConsoleMessage> m_MessageBuffer;

		bool m_EnableScrollToLatest = true;
		bool m_ScrollToLatest = false;
		float m_PreviousScrollY = 0.0f;

		int16_t m_MessageFilters = (int16_t)ConsoleMessageFlags::All;

		bool m_DetailedPanelOpen = false;
	private:
		friend class EditorConsoleSync;
	};
}
