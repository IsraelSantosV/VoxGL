#pragma once

#include "EditorPanel.h"

#include "Vox/Core/Core.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"
#include "Vox/Editor/SelectionManager.h"

#include <imgui/imgui.h>

namespace Vox
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		bool InValidContext(Ref<Scene> compareScene);
		virtual void OnRender() override;

		void DrawSelectedEntityActions();
	private:
		bool HasDrawedChild(Entity entity);
		const std::string GetEntityNameWithIcon(Entity entity);

		template<typename T>
		void DisplayAddComponentEntry(const char* icon, const std::string& entryName, bool canAddMultiples = false);

		void DrawEntityCreateMenu(const char* icon, const std::string& title, Entity parent);
		void DrawEntityNode(Entity entity, bool isChild);
		void DrawComponents(Entity entity);
		void DrawAddComponentPopup(Entity entity);

		template<typename T>
		void TryDrawComponentEntry(const char* icon, const std::string& entryName, ImGuiTextFilter filter, bool canAddMultiples = false);
	private:
		Ref<Scene> m_Context;
		std::unordered_map<UUID, Entity> m_DrawedChildren;
	};
}
