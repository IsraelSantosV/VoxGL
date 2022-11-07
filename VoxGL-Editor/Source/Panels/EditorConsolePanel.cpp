#include "VoxPch.h"
#include "EditorConsolePanel.h"
#include "Vox/Events/SceneEvents.h"
#include "Vox/Editor/FontAwesome6.h"

#include "imgui/imgui_internal.h"

namespace Vox
{
	static EditorConsolePanel* m_Instance = nullptr;

	static const ImVec4 m_InfoTint = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	static const ImVec4 m_WarningTint = ImVec4(0.8f, 0.69f, 0.05f, 1.0f);
	static const ImVec4 m_ErrorTint = ImVec4(0.8f, 0.10f, 0.10f, 1.0f);

	EditorConsolePanel::EditorConsolePanel()
	{
		VOX_CORE_ASSERT(m_Instance == nullptr);
		m_Instance = this;

		m_MessageBuffer.reserve(500);
	}

	EditorConsolePanel::~EditorConsolePanel()
	{
		m_Instance = nullptr;
	}

	void EditorConsolePanel::OnEvent(Event& event)
	{
		//EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<ScenePreStartEvent>(VOX_BIND_EVENT_FN(EditorConsolePanel::TriggerEvent));
	}

	void EditorConsolePanel::TriggerEvent(ScenePreStartEvent& e)
	{
		if (m_ClearOnPlay)
		{
			m_MessageBuffer.clear();
		}
	}

	void EditorConsolePanel::OnRender()
	{
		if (ImGui::Begin("Console"))
		{
			ImVec2 consoleSize = ImGui::GetContentRegionAvail();
			consoleSize.y -= 32.0f;

			RenderMenu({ consoleSize.x, 28.0f });
			RenderConsole(consoleSize);
		}
		ImGui::End();
	}

	void EditorConsolePanel::RenderMenu(const ImVec2& size)
	{
		ImGui::BeginChild("Toolbar", size);

		if (ImGui::Button("Clear", { 75.0f, 28.0f }))
		{
			m_MessageBuffer.clear();
		}

		ImGui::SameLine();

		const auto& style = ImGui::GetStyle();
		const std::string clearOnPlayText = fmt::format("{} Clear on Play", m_ClearOnPlay ? ICON_FA_SQUARE_CHECK : ICON_FA_SQUARE);
		ImVec4 textColor = m_ClearOnPlay ? style.Colors[ImGuiCol_Text] : style.Colors[ImGuiCol_TextDisabled];

		if (ImGui::Button(clearOnPlayText.c_str(), ImVec2(110.0f, 28.0f)))
		{
			m_ClearOnPlay = !m_ClearOnPlay;
		}

		const ImVec2 buttonSize(28.0f, 28.0f);

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100.0f, 0.0f);
		textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Info) ? m_InfoTint : style.Colors[ImGuiCol_TextDisabled];

		ImGui::PushStyleColor(ImGuiCol_Button, textColor);
		if (ImGui::Button(ICON_FA_CIRCLE_INFO, buttonSize))
		{
			m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Info;
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();
		textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Warning) ? m_WarningTint : style.Colors[ImGuiCol_TextDisabled];

		ImGui::PushStyleColor(ImGuiCol_Button, textColor);
		if (ImGui::Button(ICON_FA_CIRCLE_EXCLAMATION, buttonSize))
		{
			m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Warning;
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();
		textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Error) ? m_ErrorTint : style.Colors[ImGuiCol_TextDisabled];

		ImGui::PushStyleColor(ImGuiCol_Button, textColor);
		if (ImGui::Button(ICON_FA_BUG, buttonSize))
		{
			m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Error;
		}
		ImGui::PopStyleColor();

		ImGui::EndChild();
	}

	void EditorConsolePanel::RenderConsole(const ImVec2& size)
	{
		float scrollY = ImGui::GetScrollY();
		if (scrollY < m_PreviousScrollY)
		{
			m_EnableScrollToLatest = false;
		}

		if (scrollY >= ImGui::GetScrollMaxY())
		{
			m_EnableScrollToLatest = true;
		}

		m_PreviousScrollY = scrollY;

		ImGui::Separator();

		float rowHeight = 25.0f;
		for (uint32_t i = 0; i < m_MessageBuffer.size(); i++)
		{
			const auto& msg = m_MessageBuffer[i];

			if (!(m_MessageFilters & (int16_t)msg.Flags))
			{
				continue;
			}

			ImGui::PushID(&msg);

			ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvailWidth(), rowHeight);
			ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAvailWidth;

			std::stringstream timeString;
			tm timeBuffer;
			localtime_s(&timeBuffer, &msg.Time);
			timeString << std::put_time(&timeBuffer, "%T");

			ImGui::PushStyleColor(ImGuiCol_Text, GetMessageColor(msg));
			std::string& message = fmt::format("[{}] {}", timeString.str().c_str(), msg.ShortMessage.c_str());
			const bool clicked = ImGui::Selectable(message.c_str(), false, flags, buttonSize);
			ImGui::PopStyleColor();

			if (i == m_MessageBuffer.size() - 1 && m_ScrollToLatest)
			{
				ImGui::SetScrollFromPosY(i * rowHeight);
				m_ScrollToLatest = false;
			}

			if (clicked)
			{
				ImGui::OpenPopup("Detailed Message");

				ImVec2 size = ImGui::GetMainViewport()->Size;
				ImGui::SetNextWindowSize({ size.x * 0.5f, size.y * 0.5f });
				ImGui::SetNextWindowPos({ size.x / 2.0f, size.y / 2.5f }, 0, { 0.5, 0.5 });
				m_DetailedPanelOpen = true;
			}

			if (m_DetailedPanelOpen)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

				if (ImGui::BeginPopupModal("Detailed Message", &m_DetailedPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
				{
					std::string& longMessage = fmt::format("[{}] {}", timeString.str().c_str(), msg.LongMessage.c_str());
					ImGui::TextWrapped(longMessage.c_str());
					ImGui::EndPopup();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			ImGui::PopID();
		}
	}

	const char* EditorConsolePanel::GetMessageType(const ConsoleMessage& message) const
	{
		if (message.Flags & (int16_t)ConsoleMessageFlags::Info) return "Info";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Warning) return "Warning";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Error) return "Error";
		return "Unknown";
	}

	const ImVec4& EditorConsolePanel::GetMessageColor(const ConsoleMessage& message) const
	{
		if (message.Flags & (int16_t)ConsoleMessageFlags::Info) return m_InfoTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Warning) return m_WarningTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Error) return m_ErrorTint;
		return m_InfoTint;
	}

	ImVec4 EditorConsolePanel::GetToolbarButtonColor(const bool value) const
	{
		const auto& style = ImGui::GetStyle();
		return value ? style.Colors[ImGuiCol_Header] : style.Colors[ImGuiCol_FrameBg];
	}

	void EditorConsolePanel::PushMessage(const ConsoleMessage& message)
	{
		if (m_Instance == nullptr)
		{
			return;
		}

		m_Instance->m_MessageBuffer.push_back(message);

		if (m_Instance->m_EnableScrollToLatest)
		{
			m_Instance->m_ScrollToLatest = true;
		}
	}
}
