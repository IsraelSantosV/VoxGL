#include "VoxPch.h"
#include "Vox/Scene/Scene.h"
#include "Vox/Scene/Entity.h"

#include "Vox/Scene/Components.h"
#include "Vox/Scene/ScriptableEntity.h"
#include "Vox/Scripting/ScriptEngine.h"
#include "Vox/Renderer/Renderer2D.h"

#include "Vox/Physics/2D/Physics2D.h"

#include <glm/glm.hpp>

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Vox
{
	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		VOX_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	std::unordered_map<UUID, Scene*> m_ActiveScenes;

	struct SceneComponent
	{
		UUID SceneId;
	};

	Scene::Scene(std::string name, bool isEditorScene, bool initialize) 
		: m_Name(name), m_EditorScene(isEditorScene)
	{
		m_SceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);
		m_ActiveScenes[m_SceneID] = this;

		if (!initialize)
		{
			return;
		}

		Box2DWorldComponent& b2dWorld = m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, CreateScope<b2World>(b2Vec2{ 0.0f, -9.8f }));
		b2dWorld.World->SetContactListener(&b2dWorld.ContactListener);

		Init();
	}

	Scene::~Scene()
	{
		m_Registry.clear();
		m_ActiveScenes.erase(m_SceneID);
	}

	void Scene::Init()
	{
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).Id);

					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
				{
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).Id;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithId(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		// Scripting
		{
			ScriptEngine::OnRuntimeStart(this);

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		OnPhysics2DStop();
		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			UpdateBehaviours(ts);
			UpdatePhysics(ts);
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		auto view = m_Registry.view<IDComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto& [idComponent, camera] = view.get<IDComponent, CameraComponent>(entity);
			Entity cameraEntity = GetEntityWithId(idComponent.Id);

			if (camera.MainCamera && cameraEntity.IsActive())
			{
				mainCamera = &camera.Camera;
				cameraTransform = cameraEntity.Transform().GetTransform();
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			// Draw sprites
			{
				auto group = m_Registry.group<IDComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [idComponent, sprite] = group.get<IDComponent, SpriteRendererComponent>(entity);
					Entity entity = GetEntityWithId(idComponent.Id);

					if (entity && entity.IsActive())
					{
						glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
						Renderer2D::DrawSprite(transform, sprite, (int)entity);
					}
				}
			}

			// Draw circles
			{
				auto view = m_Registry.view<IDComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [idComponent, circle] = view.get<IDComponent, CircleRendererComponent>(entity);
					Entity entity = GetEntityWithId(idComponent.Id);

					if (entity && entity.IsActive())
					{
						glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
						Renderer2D::DrawCircle(transform, circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			UpdatePhysics(ts);
		}

		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderScene(camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
		{
			return;
		}

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			Entity currentEntity = Entity{ entity, this };
			if (!currentEntity.IsActive()) continue;

			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.MainCamera)
			{
				return currentEntity;
			}
		}

		return {};
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	Entity Scene::CreateEntityWithId(UUID id, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<IDComponent>(id);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<VisibilityComponent>(true);
		entity.AddComponent<LayerComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "New Entity" : name;

		entity.AddComponent<RelationshipComponent>();

		m_EntityMap[id] = entity;
		//SortEntities();
		return entity;
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		Entity entity = CreateEntityWithId(UUID(), name);

		if (parent)
		{
			entity.SetParent(parent);
		}

		return entity;
	}

	void Scene::DestroyEntity(Entity entity, bool excludeChildren, bool first)
	{
		if (!m_EditorScene)
		{
			if (entity.HasComponent<Rigidbody2DComponent>())
			{
				auto& world = m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World;
				b2Body* body = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
				world->DestroyBody(body);
			}
		}

		if (!excludeChildren)
		{
			for (size_t i = 0; i < entity.Children().size(); i++)
			{
				auto childId = entity.Children()[i];
				Entity child = GetEntityWithId(childId);
				DestroyEntity(child, excludeChildren, false);
			}
		}

		if (first)
		{
			if (auto parent = entity.GetParent(); parent)
			{
				parent.RemoveChild(entity);
			}
		}

		UUID id = entity.GetId();

		m_Registry.destroy(entity.m_EntityHandle);
		m_EntityMap.erase(id);

		//SortEntities();
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		auto parentNewEntity = [&entity, scene = this](Entity newEntity)
		{
			if (auto parent = entity.GetParent(); parent)
			{
				newEntity.SetParentId(parent.GetId());
				parent.Children().push_back(newEntity.GetId());
			}
		};

		Entity newEntity = CreateEntity(entity.GetName());
		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		auto childrenId = entity.Children();
		for (auto childId : childrenId)
		{
			Entity childDuplicate = DuplicateEntity(GetEntityWithId(childId));
			UnparentEntity(childDuplicate, false);

			childDuplicate.SetParentId(newEntity.GetId());
			newEntity.Children().push_back(childDuplicate.GetId());
		}

		parentNewEntity(newEntity);
		return newEntity;
	}

	Entity Scene::GetEntityWithId(UUID id)
	{
		if (m_EntityMap.find(id) != m_EntityMap.end())
		{
			return { m_EntityMap.at(id), this };
		}

		return {};
	}

	Entity Scene::FindEntityWithId(UUID id)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			const IDComponent& tc = view.get<IDComponent>(entity);
			if (tc.Id == id)
			{
				return Entity { entity, this };
			}
		}

		return {};
	}

	Entity Scene::FindEntityWithTag(const std::string& tag)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == tag)
			{
				return Entity { entity, this };
			}
		}

		return {};
	}

	Entity Scene::FindChildEntityWithTag(Entity entity, const std::string& tag)
	{
		if (entity)
		{
			if (entity.GetComponent<TagComponent>().Tag == tag)
			{
				return entity;
			}

			for (const auto childId : entity.Children())
			{
				Entity descendant = FindChildEntityWithTag(GetEntityWithId(childId), tag);
				if (descendant)
				{
					return descendant;
				}
			}
		}
		return {};
	}

	void Scene::SortEntities()
	{
		m_Registry.sort<IDComponent>([&](const auto lhs, const auto rhs)
			{
				auto lhsEntity = m_EntityMap.find(lhs.Id);
				auto rhsEntity = m_EntityMap.find(rhs.Id);
				return static_cast<uint32_t>(lhsEntity->second) < static_cast<uint32_t>(rhsEntity->second);
			});
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		Entity parent = FindEntityWithId(entity.GetParentId());

		if (!parent)
		{
			return;
		}

		auto& transform = entity.Transform();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		transform.SetTransform(localTransform);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		Entity parent = FindEntityWithId(entity.GetParentId());

		if (!parent)
		{
			return;
		}

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.Transform();
		entityTransform.SetTransform(transform);
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		glm::mat4 transform(1.0f);

		Entity parent = FindEntityWithId(entity.GetParentId());
		if (parent)
		{
			transform = GetWorldSpaceTransformMatrix(parent);
		}

		return transform * entity.Transform().GetTransform();
	}

	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		TransformComponent transformComponent;
		transformComponent.SetTransform(transform);
		return transformComponent;
	}

	void Scene::SetParentEntity(Entity entity, Entity parent)
	{
		if (parent.IsChildOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = FindEntityWithId(entity.GetParentId());
			if (newParent)
			{
				UnparentEntity(entity);
				SetParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = FindEntityWithId(entity.GetParentId());

			if (previousParent)
				UnparentEntity(entity);
		}

		entity.SetParentId(parent.GetId());
		parent.Children().push_back(entity.GetId());

		ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
	{
		Entity parent = FindEntityWithId(entity.GetParentId());
		if (!parent)
		{
			return;
		}

		auto& parentChildren = parent.Children();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetId()), parentChildren.end());

		if (convertToWorldSpace)
		{
			ConvertToWorldSpace(entity);
		}

		entity.SetParentId(0);
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::UpdateBehaviours(Timestep ts)
	{
		// C# Entity OnUpdate
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			if (!entity.IsActive()) continue;
			ScriptEngine::OnUpdateEntity(entity, ts);
		}

		m_Registry.view<BehaviourComponent>().each([=](auto entity, auto& behaviour)
		{
			if (!behaviour.Instance)
			{
				//auto targetEntity = Entity{ entity, this };
				behaviour.Instance = behaviour.InstantiateScript();
				behaviour.Instance->m_Entity = Entity{ entity, this };

				behaviour.Instance->OnCreate();
			}

			//if (targetEntity.IsActive())
			//{
			behaviour.Instance->OnUpdate(ts);
			//}
		});
	}

	void Scene::UpdatePhysics(Timestep ts)
	{
		auto sceneView = m_Registry.view<Box2DWorldComponent>();
		auto& box2DWorld = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;
		const uint32_t velocityIterations = 6;
		const uint32_t positionIterations = 2;

		box2DWorld->Step(ts, velocityIterations, positionIterations);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			if (!entity.IsActive()) continue;

			auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();
			if (rb2D.RuntimeBody == nullptr)
			{
				continue;
			}

			b2Body* body = static_cast<b2Body*>(rb2D.RuntimeBody);
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& position = body->GetPosition();

			transform.Position.x = position.x;
			transform.Position.y = position.y;
			glm::vec3 rotation = transform.GetRotationEuler();
			rotation.z = body->GetAngle();
			transform.SetRotationEuler(rotation);
		}
	}

	void Scene::OnPhysics2DStart()
	{
		auto sceneView = m_Registry.view<Box2DWorldComponent>();
		auto& world = m_Registry.get<Box2DWorldComponent>(sceneView.front()).World;

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			UUID entityId = entity.GetId();
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.GetRotationEuler().z;

			b2Body* body = world->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);

			b2MassData massData;
			body->GetMassData(&massData);
			massData.mass = rb2d.Mass;
			body->SetMassData(&massData);
			body->SetGravityScale(rb2d.GravityScale);
			body->SetLinearDamping(rb2d.LinearDrag);
			body->SetAngularDamping(rb2d.AngularDrag);
			body->SetBullet(rb2d.IsBullet);
			body->GetUserData().pointer = (uintptr_t)entityId;
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, 
					b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprites
		{
			auto group = m_Registry.group<IDComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [idComponent, sprite] = group.get<IDComponent, SpriteRendererComponent>(entity);
				Entity entity = GetEntityWithId(idComponent.Id);

				if (entity && entity.IsActive())
				{
					glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
					Renderer2D::DrawSprite(transform, sprite, (int)entity);
				}
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<IDComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [idComponent, circle] = view.get<IDComponent, CircleRendererComponent>(entity);
				Entity entity = GetEntityWithId(idComponent.Id);

				if (entity && entity.IsActive())
				{
					glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
					Renderer2D::DrawCircle(transform, circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}
		}

		Renderer2D::EndScene();
	}

	bool Scene::EntityIsScene(Entity entity)
	{
		if (entity)
		{
			return entity.HasComponent<SceneComponent>();
		}

		return false;
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<VisibilityComponent>(Entity entity, VisibilityComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<LayerComponent>(Entity entity, LayerComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BehaviourComponent>(Entity entity, BehaviourComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}
}
