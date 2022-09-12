#pragma once

#include "Vox.h"

#include "Panels/SceneHierarchyPanel.h"

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
		void DrawGizmos();
		bool OnKeyPressed(KeyPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmosType = 0;
		int m_GizmosSpace = 0;
		float m_SnapValue = 0.5f;
		float m_RotationSnap = 15.0f;

		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
