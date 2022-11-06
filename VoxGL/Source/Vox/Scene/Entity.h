#pragma once

#include "Vox/Core/UUID.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Components.h"

#include "entt.hpp"

namespace Vox
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			VOX_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			VOX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent() const
		{
			VOX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			VOX_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		UUID GetId() { return GetComponent<IDComponent>().Id; }
		std::string& GetName() { return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : EmptyName; }
		const std::string& GetName() const { return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : EmptyName; }

		TransformComponent& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
		const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle).GetTransform(); }

		void SetParentId(UUID parent) { GetComponent<RelationshipComponent>().ParentHandle = parent; }
		UUID GetParentId() const { return GetComponent<RelationshipComponent>().ParentHandle; }
		std::vector<UUID>& Children() { return GetComponent<RelationshipComponent>().Children; }
		const std::vector<UUID>& Children() const { return GetComponent<RelationshipComponent>().Children; }

		Entity GetParent() const
		{
			return m_Scene->GetEntityWithId(GetParentId());
		}

		void SetParent(Entity parent)
		{
			Entity currentParent = GetParent();
			if (currentParent == parent)
			{
				return;
			}

			if (currentParent)
			{
				currentParent.RemoveChild(*this);
			}

			SetParentId(parent.GetId());

			if (parent)
			{
				auto& parentChildren = parent.Children();
				UUID uuid = GetId();
				if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
				{
					parentChildren.emplace_back(GetId());
				}
			}
		}

		bool RemoveChild(Entity child)
		{
			UUID childId = child.GetId();
			std::vector<UUID>& children = Children();
			auto it = std::find(children.begin(), children.end(), childId);
			if (it != children.end())
			{
				children.erase(it);
				return true;
			}

			return false;
		}

		bool IsParentOf(Entity entity) const
		{
			const auto& children = Children();

			if (children.empty())
			{
				return false;
			}

			for (UUID child : children)
			{
				if (child == entity.GetId())
				{
					return true;
				}
			}

			for (UUID child : children)
			{
				if (m_Scene->GetEntityWithId(child).IsParentOf(entity))
				{
					return true;
				}
			}

			return false;
		}

		bool IsChildOf(Entity entity) const { return entity.IsParentOf(*this); }

		bool IsActive() { return m_IsActive; }
		void SetActive(bool enable) { m_IsActive = enable; }

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		inline static std::string EmptyName = "Empty Entity";

		bool m_IsActive = true;
		Scene* m_Scene = nullptr;
	};
}
