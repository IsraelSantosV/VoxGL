#pragma once

#include "Vox.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/EditorConsolePanel.h"

#include "Vox/Renderer/EditorCamera.h"

namespace Vox
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		void RenderGrid();
		void MousePicking();
		void DrawGizmos();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnOverlayRender();
		void RenderOutline();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();

		void OnDuplicateEntity();

		void UI_Toolbar();
		void UI_Settings();	
	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		EditorCamera m_EditorCamera;

		Entity m_HoveredEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		bool m_EnableGizmosInRuntime = false;

		bool m_EnableGrid = true;
		bool m_ShowStyleDemo = false;
		bool m_ShowImGuiDrawDemo = false;
		bool m_ShowPhysicsColliders = true;
		int m_GizmosType = 0;
		int m_GizmosSpace = 0;
		float m_SnapValue = 0.5f;
		float m_RotationSnap = 15.0f;

		enum class SceneState
		{
			Edit = 0, 
			Play = 1, 
			Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		Ref<Texture2D> m_IconPlay, m_IconStop, m_IconSimulate;

		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		EditorConsolePanel m_EditorConsolePanel;
	};
}
