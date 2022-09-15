#pragma once
#include "EditorPanel.h"

#include "Vox/Renderer/Texture.h"

#include <filesystem>

namespace Vox 
{
	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnRender() override;
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_EmptyDirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
