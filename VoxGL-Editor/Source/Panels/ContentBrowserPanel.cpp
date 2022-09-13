#include "VoxPch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Vox 
{
	extern const std::filesystem::path m_AssetPath = "Assets";

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(m_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/Folder.png");
		m_EmptyDirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/EmptyFolder.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/File.png");
	}

	void ContentBrowserPanel::OnRender()
	{
		ImGui::Begin("Project");

		if (m_CurrentDirectory != std::filesystem::path(m_AssetPath))
		{
			if (ImGui::Button("Back", ImVec2{ 100, 35 }))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);

		if (columnCount < 1)
		{
			columnCount = 1;
		}

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			
			std::string filenameString = path.filename().string();
			
			ImGui::PushID(filenameString.c_str());

			bool isDirectory = directoryEntry.is_directory();
			Ref<Texture2D> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::ImageButton((ImTextureID)icon->GetRendererId(), { thumbnailSize, thumbnailSize }, { 0,1 }, { 1,0 });

			if (ImGui::BeginDragDropSource())
			{
				auto relativePath = std::filesystem::relative(path, m_AssetPath);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
				{
					m_CurrentDirectory /= path.filename();
				}
			}

			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();

			ImGui::PopID();
		}

		//ImGui::Columns(1);

		//ImGui::SliderFloat("Icon Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}

}
