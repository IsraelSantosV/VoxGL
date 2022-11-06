#pragma once

#include "EditorPanel.h"

#include "Vox/Core/Core.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"
#include "Vox/Editor/SelectionManager.h"

namespace Vox
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		virtual void OnRender() override;

		void DrawSelectedEntityActions();
	private:
		bool TagSearchRecursive(Entity entity, std::string_view searchFilter, uint32_t maxSearchDepth, uint32_t currentDepth = 1);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName, bool canAddMultiples = false);

		void DrawEntityCreateMenu(const std::string& title, Entity parent);
		void DrawEntityNode(Entity entity, const std::string& searchFilter = {});
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		std::unordered_map<UUID, Entity> m_HierarchyEntities;
	};
}
