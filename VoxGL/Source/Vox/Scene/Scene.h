#pragma once

#include "entt.hpp"

#include "Vox/Core/Timestep.h"
#include "Vox/Core/UUID.h"
#include "Vox/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;

namespace Vox
{
	class Entity;
	class TransformComponent;

	class Scene
	{
	public:
		Scene(std::string name = "Empty Scene");
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetMainCameraEntity();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithId(UUID id, const std::string& name = std::string());
		Entity CreateChildEntity(Entity parent, const std::string& name);
		void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);
		Entity DuplicateEntity(Entity entity);

		Entity GetEntityWithId(UUID id) const;
		Entity FindEntityWithId(UUID id) const;
		Entity FindEntityWithTag(const std::string& tag);
		Entity FindChildEntityWithTag(Entity entity, const std::string& tag);
		void SortEntities();

		template<typename Fn>
		void SubmitPostUpdateFunc(Fn&& func)
		{
			m_PostUpdateQueue.emplace_back(func);
		}

		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
		glm::mat4 GetWorldSpaceTransformMatrix(Entity entity);
		TransformComponent GetWorldSpaceTransform(Entity entity);

		void SetParentEntity(Entity entity, Entity parent);
		void UnparentEntity(Entity entity, bool convertToWorldSpace = true);

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		bool IsRunning() const { return m_IsRunning; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void UpdateBehaviours(Timestep ts);
		void UpdatePhysics(Timestep ts);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);
	private:
		std::string& m_Name;
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		bool m_IsRunning = false;

		b2World* m_PhysicsWorld = nullptr;
		std::unordered_map<UUID, Entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
