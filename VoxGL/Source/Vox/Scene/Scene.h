#pragma once

#include "entt.hpp"

#include "Vox/Core/Timestep.h"
#include "Vox/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;

namespace Vox
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetMainCameraEntity();
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void UpdateBehaviours(Timestep ts);
		void UpdatePhysics(Timestep ts);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
