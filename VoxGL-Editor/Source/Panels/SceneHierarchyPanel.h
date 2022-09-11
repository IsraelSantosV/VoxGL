#pragma once

#include "Vox/Core/Core.h"
#include "Vox/Core/Log.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"

namespace Vox
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void OnRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
