#pragma once

#include "EditorPanel.h"

#include "Vox/Core/Core.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"

namespace Vox
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SelectEntity(Entity entity);

		void SetContext(const Ref<Scene>& scene);
		virtual void OnRender() override;
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
